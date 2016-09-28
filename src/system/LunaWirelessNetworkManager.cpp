#include "ccCore/ccString.h"

#include <iostream>
#include <vector>
#include <string>

#include "LunaWirelessNetworkManager.h"
#include "LunaSystemCall.h"

#include "ccJsonParser/json/writer.h"

/*
nmcli d                   
DEVICE           TYPE      STATE         CONNECTION       
eth0             ethernet  connected     Local Connection 
wlx0013efc21589  wifi      disconnected  --               
eth1             ethernet  unavailable   --               
lo               loopback  unmanaged     --  

nmcli d show eth0 | grep IP4
IP4.ADDRESS[1]:                         10.0.1.15/24
IP4.GATEWAY:                            10.0.1.1
IP4.ROUTE[1]:                           dst = 169.254.0.0/16, nh = 0.0.0.0, mt = 1000
IP4.DNS[1]:                             10.0.1.1
IP4.DOMAIN[1]:                          kornet

nmcli -f device,type,state d | grep wifi
wlx0013efc21589  wifi      connected

nmcli -f ssid,chan,signal,security d wifi list ifname 'wlx0013efc21589'
SSID          CHAN  SIGNAL  SECURITY
iptimejry     1     55      WPA2
T wifi home   5     55      WPA1 WPA2 802.1X
SK_WiFiD8A6   5     55      WPA1 WPA2
MarsNetworks  11    47      WPA2

sudo nmcli d wifi connect 'MarsNetworks' password 'ms0428jw!' ifname 'wlx0013efc21589'
*/

template<typename T>
std::vector<T> split_token(const std::string& source, const std::string& separator) {
    std::vector<T> tokens;
    
    auto start = 0U;
    auto end = source.find(separator);

    while (end != std::string::npos) {

        tokens.emplace_back(source.substr(start, end - start));

        start = end + separator.length();
        end = source.find(separator, start);
    }

    tokens.emplace_back(source.substr(start, end));

    return tokens;
}

LunaWirelessNetworkManager::LunaWirelessNetworkManager() {
}

Json::Value LunaWirelessNetworkManager::get_info(const std::string& finding_ifname) {
    Json::Value if_list;

    if_list = get_if_list(finding_ifname);

    for (Json::Value& it : if_list["if_list"])  {        
        Json::Value if_ssid_list = get_ssid_list(it["ifname"].asCString());
        Json::Value if_ipv4_info = get_ipv4_info(it["connection_name"].asString());
        Json::Value if_wireless_info = get_wireless_info(it["connection_name"].asString());
        
        it["ipv4"] = if_ipv4_info;
        it["wireless"] = if_wireless_info;

        it["ssid_list"] = if_ssid_list;
    }

#ifdef _WIN32
        Json::StyledWriter json_writer;
        std::string json_data = json_writer.write(if_list);

        std::cout << "json_data: {\n" << json_data << "\n}" << std::endl;
#endif // _WIN32

    return if_list;
}

/*
    nmcli d | grep 'wifi\|CONNECTION'
    DEVICE           TYPE      STATE        CONNECTION       
    wlx0013efc21589  wifi      connected    MarsNetworks
*/

Json::Value LunaWirelessNetworkManager::get_if_list(const std::string& finding_ifname) {
    std::string result = Luna_system_exec("export LC_ALL=C\nnmcli d | grep 'wifi\\|CONNECTION'");

#ifdef _WIN32
    result =
        "DEVICE           TYPE      STATE        CONNECTION\n" \
        "wlx0013efc21589  wifi      connected    MarsNetworks\n";
#endif // _WIN32

    std::string find_new_line_pattern = "\n";
    std::string find_title_line_pattern1 = "TYPE";
    std::string find_title_line_pattern2 = "CONNECTION";
    std::string line_buffer;

    std::size_t item_pos[4] = { 0, };

    std::size_t start_pos = 0;
    std::size_t find_pos = 0;

    Json::Value if_list;

    while (find_pos != std::string::npos) {
        find_pos = result.find(find_new_line_pattern, start_pos);

        if (find_pos != std::string::npos) {
            line_buffer = result.substr(start_pos, find_pos - start_pos);

            start_pos = find_pos + find_new_line_pattern.length();
        } else {
            line_buffer = result.substr(start_pos, result.length() - start_pos);
        }

        if (line_buffer.length() < 10)
            continue;

        if (line_buffer.find(find_title_line_pattern1) != std::string::npos && line_buffer.find(find_title_line_pattern2) != std::string::npos) {
            item_pos[1] = result.find("TYPE", 0);
            item_pos[2] = result.find("STATE", 0);
            item_pos[3] = result.find("CONNECTION", 0);
        } else {
            Luna::ccString ifname = line_buffer.substr(item_pos[0], item_pos[1] - 1);
            Luna::ccString type = line_buffer.substr(item_pos[1], (item_pos[2] - 1) - item_pos[1]);
            Luna::ccString state = line_buffer.substr(item_pos[2], (item_pos[3] - 1) - item_pos[2]);
            Luna::ccString connection_name = line_buffer.substr(item_pos[3]);

            ifname.trim_right();
            state.trim_right();
            connection_name.trim_right();

            if (finding_ifname == "" || finding_ifname == ifname) {
                Json::Value if_state;

                if_state["ifname"] = ifname;
                if_state["state"] = state;
                if_state["connection_name"] = connection_name;

                if_list["if_list"].append(if_state);
            }
        }    
    }

    return if_list;
}

//

/*
    nmcli -f ssid,chan,signal,security d wifi list ifname 'wlx0013efc21589'
    SSID          CHAN  SIGNAL  SECURITY
    iptimejry     1     55      WPA2
    T wifi home   5     55      WPA1 WPA2 802.1X
    SK_WiFiD8A6   5     55      WPA1 WPA2
    MarsNetworks  11    47      WPA2
*/

Json::Value LunaWirelessNetworkManager::get_ssid_list(const std::string& finding_ifname) {
    std::string scan_command;

    Luna::ccString::format(
        scan_command,
        "export LC_ALL=C\n"\
        "nmcli -f ssid,chan,signal,security d wifi list ifname '%s'",
        finding_ifname.c_str()
    );
    
    std::string result = Luna_system_exec(scan_command);

#ifdef _WIN32
    result = "" \
        "SSID          CHAN  SIGNAL  SECURITY\n" \
        "iptimejry     1     55      WPA2\n" \
        "T wifi home   5     55      WPA1 WPA2 802.1X\n" \
        "SK_WiFiD8A6   5     55      WPA1 WPA2\n" \
        "MarsNetworks  11    47      WPA2\n";

#endif // _WIN32

    std::string find_new_line_pattern = "\n";
    std::string find_title_line_pattern = "SECURITY";
    std::string line_buffer;

    std::size_t item_pos[4] = { 0, };

    std::size_t start_pos = 0;
    std::size_t find_pos = 0;

    Json::Value ssid_list;

    while (find_pos != std::string::npos) {
        find_pos = result.find(find_new_line_pattern, start_pos);

        if (find_pos != std::string::npos) {
            line_buffer = result.substr(start_pos, find_pos - start_pos);

            start_pos = find_pos + find_new_line_pattern.length();
        } else {
            line_buffer = result.substr(start_pos, result.length() - start_pos);
        }

        if (line_buffer.length() < 10)
            continue;

        if (line_buffer.find(find_title_line_pattern) != std::string::npos) {
            item_pos[1] = result.find("CHAN", 0);
            item_pos[2] = result.find("SIGNAL", 0);
            item_pos[3] = result.find("SECURITY", 0);
        } else {
            Luna::ccString ssid = line_buffer.substr(item_pos[0], item_pos[1] - 1);
            Luna::ccString channel = line_buffer.substr(item_pos[1], (item_pos[2] - 1) - item_pos[1]);
            Luna::ccString signal = line_buffer.substr(item_pos[2], (item_pos[3] - 1) - item_pos[2]);
            Luna::ccString security = line_buffer.substr(item_pos[3]);

            ssid.trim_right();
            channel.trim_right();
            signal.trim_right();
            security.trim_right();

            Json::Value ssid_info;

            ssid_info["ssid"] = ssid;
            ssid_info["channel"] = channel;
            ssid_info["signal"] = signal;
            ssid_info["security"] = security;

            ssid_list.append(ssid_info);
        }
    }

    return ssid_list;
}

/*
    nmcli c show 'MarsNetworks' | grep '802-11-wireless.ssid\|ipv4.method\|IP4'
    802-11-wireless.ssid:                   MarsNetworks
    ipv4.method:                            auto
    IP4.ADDRESS[1]:                         10.0.1.16/24
    IP4.GATEWAY:                            10.0.1.1
    IP4.DNS[1]:                             10.0.1.1
    IP4.DOMAIN[1]:                          kornet

*/

Json::Value LunaWirelessNetworkManager::get_ipv4_info(const std::string& connection_name) {
    std::string scan_command;

    Luna::ccString::format(
        scan_command,
        "export LC_ALL=C\n"\
        "nmcli c show '%s' | grep 'ipv4.method\\|IP4'",
        connection_name.c_str()
    );
    
    std::string result = Luna_system_exec(scan_command);

#ifdef _WIN32
    result = "" \
        "ipv4.method:                            auto\n" \
        "IP4.ADDRESS[1]:                         10.0.1.15/24\n" \
        "IP4.GATEWAY:                            10.0.1.1\n" \
        "IP4.ROUTE[1]:                           dst = 169.254.0.0/16, nh = 0.0.0.0, mt = 1000\n" \
        "IP4.DNS[1]:                             10.0.1.1\n" \
        "IP4.DOMAIN[1]:                          kornet\n";
#endif // _WIN32

    std::string find_new_line_pattern = "\n";
    std::string line_buffer;

    std::size_t start_pos = 0;
    std::size_t find_pos = 0;

    Json::Value if_ipv4_info;

    if_ipv4_info["method"] = "-";
    if_ipv4_info["address"] = "-";
    if_ipv4_info["subnet_mask"] = "-";
    if_ipv4_info["gateway"] = "-";
    if_ipv4_info["dns"] = "-";

    while (find_pos != std::string::npos) {
        find_pos = result.find(find_new_line_pattern, start_pos);

        if (find_pos != std::string::npos) {
            line_buffer = result.substr(start_pos, find_pos - start_pos);

            start_pos = find_pos + find_new_line_pattern.length();
        } else {
            line_buffer = result.substr(start_pos, result.length() - start_pos);
        }

        if (line_buffer.length() < 10)
            continue;

        if (line_buffer.find("ipv4.method") != std::string::npos) {
            auto tokens = split_token<Luna::ccString>(line_buffer, ":");

            if (tokens.size() == 2) {
                tokens[1].trim();

                if (tokens[1] == "auto")
                    if_ipv4_info["method"] = "dhcp";
                else
                    if_ipv4_info["method"] = "static";
            }

            continue;
        } 

        if (line_buffer.find("IP4.ADDRESS[") != std::string::npos) {
            auto tokens = split_token<Luna::ccString>(line_buffer, "]:");

            if (tokens.size() == 2) {
                auto sub_tokens = split_token<Luna::ccString>(tokens[1], "/");

                if (sub_tokens.size() == 2) {
                    sub_tokens[0].trim();
                    sub_tokens[1].trim();

                    if_ipv4_info["address"] = sub_tokens[0];

                    switch (atoi(sub_tokens[1].c_str())) {
                    case  8: if_ipv4_info["subnet_mask"] = "255.0.0.0";  break;
                    case 16: if_ipv4_info["subnet_mask"] = "255.255.0.0";  break;

                    default:
                    case 24: if_ipv4_info["subnet_mask"] = "255.255.255.0";  break;
                    }
                }
            }

            continue;
        }

        if (line_buffer.find("IP4.GATEWAY") != std::string::npos) {
            auto tokens = split_token<Luna::ccString>(line_buffer, ":");

            if (tokens.size() == 2) {
                tokens[1].trim();
                if_ipv4_info["gateway"] = tokens[1];
            }

            continue;
        } 

        if (line_buffer.find("IP4.DNS[") != std::string::npos) {
            auto tokens = split_token<Luna::ccString>(line_buffer, "]:");

            if (tokens.size() == 2) {
                tokens[1].trim();
                if_ipv4_info["dns"] = tokens[1];
            }

            continue;
        }
    }

    return if_ipv4_info;
}

Json::Value LunaWirelessNetworkManager::get_wireless_info(const std::string& connection_name) {
    std::string scan_command;

    Luna::ccString::format(
        scan_command,
        "export LC_ALL=C\n"\
        "nmcli c show '%s' | grep '802-11-wireless.ssid'",
        connection_name.c_str()
    );
    
    std::string result = Luna_system_exec(scan_command);

#ifdef _WIN32
    result = "" \
        "802-11-wireless.ssid:                   MarsNetworks\n";
#endif // _WIN32

    std::string find_new_line_pattern = "\n";
    std::string line_buffer;

    std::size_t start_pos = 0;
    std::size_t find_pos = 0;

    Json::Value if_wireless_info;

    if_wireless_info["ssid"] = "";

    while (find_pos != std::string::npos) {
        find_pos = result.find(find_new_line_pattern, start_pos);

        if (find_pos != std::string::npos) {
            line_buffer = result.substr(start_pos, find_pos - start_pos);

            start_pos = find_pos + find_new_line_pattern.length();
        } else {
            line_buffer = result.substr(start_pos, result.length() - start_pos);
        }

        if (line_buffer.length() < 10)
            continue;

        if (line_buffer.find("802-11-wireless.ssid") != std::string::npos) {
            auto tokens = split_token<Luna::ccString>(line_buffer, ":");

            if (tokens.size() == 2) {
                tokens[1].trim();
                if_wireless_info["ssid"] = tokens[1];
            }

            continue;
        } 
    }

    return if_wireless_info;
}