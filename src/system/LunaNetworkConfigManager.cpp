#include "ccCore/ccString.h"

#include <iostream>
#include <vector>
#include <string>

#include "LunaNetworkConfigManager.h"
#include "LunaSystemCall.h"

#include "json/writer.h"


LunaNetworkConfigManager::LunaNetworkConfigManager() {
}

Json::Value LunaNetworkConfigManager::get_ethernet_if_list(const std::string& ifname) {
    Json::Value if_list = get_if_list_impl(false, ifname);

    for (Json::Value& it : if_list["if_list"])  {
        Json::Value if_ipv4_info = get_ipv4_info(it["connection_name"].asString());
        
        it["ipv4"] = if_ipv4_info;
   }

    return if_list;
}

Json::Value LunaNetworkConfigManager::get_wireless_if_list(const std::string& ifname) {
    Json::Value if_list = get_if_list_impl(true, ifname);

     for (Json::Value& it : if_list["if_list"])  {
        Json::Value if_ipv4_info = get_ipv4_info(it["connection_name"].asString());
        Json::Value if_wireless_info = get_wireless_info(it["connection_name"].asString());
        
        it["ipv4"] = if_ipv4_info;
        it["wireless"] = if_wireless_info;
   }

    return if_list;    
}

/*
    nmcli -f uuid,type,device c | grep 'ethernet.*--'
    fb895358-84bf-408d-982c-a00b6e47f03d  802-3-ethernet   -- 
*/

std::vector<Luna::ccString> 
LunaNetworkConfigManager::look_for_disconnected_connection_uuid_list() {
    std::string sys_command;

    Luna::ccString::format(
        sys_command,
        "export LC_ALL=C\n" \
        "nmcli -f uuid,type,device c | grep 'ethernet.*--'"
    );

    std::string result = Luna_system_exec(sys_command);

#if defined(_WIN32) || defined(__APPLE__)
    result =
        "fb895358-84bf-408d-982c-a00b6e47f03d  802-3-ethernet   --\n";
#endif // _WIN32 || __APPLE__ 

    std::string find_new_line_pattern = "\n";
    std::string line_buffer;

    std::size_t start_pos = 0;
    std::size_t find_pos = 0;

    std::vector<Luna::ccString>  uuid_list;

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

        if (line_buffer.find("802-3-ethernet") != std::string::npos) {
            auto tokens = split_token<Luna::ccString>(line_buffer, "802-3-ethernet");

            if (tokens.size() == 2) {
                tokens[0].trim();

                uuid_list.push_back(tokens[0]);
            }
        } 
    }

    return uuid_list;
}

/*
    nmcli c show fb895358-84bf-408d-982c-a00b6e47f03d | grep 'connection.uuid\|connection.interface-name'
    connection.uuid:                        fb895358-84bf-408d-982c-a00b6e47f03d
    connection.interface-name:              eth1
*/

Json::Value LunaNetworkConfigManager::look_for_connection_uuid_assciated_ifname(const std::string& ifname) {
    std::vector<Luna::ccString> uuid_list = look_for_disconnected_connection_uuid_list();
    
    std::string sys_command;
    Json::Value con_info;

    for (auto it : uuid_list) {
        Luna::ccString::format(
            sys_command,
            "export LC_ALL=C\n" \
            "nmcli c show '%s' | grep 'connection.id\\|connection.interface-name'",
            it.c_str()
        );

        std::string result = Luna_system_exec(sys_command);

#if defined(_WIN32) || defined(__APPLE__)
        result =
            "connection.id:                          TFTP/HTTP Connection" \
            "connection.interface-name:              eth1\n";
#endif // _WIN32 || __APPLE__


        std::string find_new_line_pattern = "\n";
        std::string line_buffer;

        std::size_t start_pos = 0;
        std::size_t find_pos = 0;

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

            if (line_buffer.find("connection.id:") != std::string::npos) {
                auto tokens = split_token<Luna::ccString>(line_buffer, "onnection.id: ");

                if (tokens.size() == 2) {
                    tokens[1].trim();
                    con_info["name"] = tokens[1];
                }
            }

            if (line_buffer.find("connection.interface-name:") != std::string::npos) {
                auto tokens = split_token<Luna::ccString>(line_buffer, ".interface-name: ");

                if (tokens.size() == 2) {
                    tokens[1].trim();

                    if (tokens[1] == ifname) {
                        con_info["uuid"] = it;                 
                        return con_info;
                    }
                }
            } 
        }
    }

    con_info["name"] = "";
    con_info["uuid"] = "";
    
    return con_info;
}


Json::Value LunaNetworkConfigManager::get_if_list_impl(bool is_wireless_if, const std::string& ifname) {
    std::string sys_command;

    if (is_wireless_if) {
        Luna::ccString::format(
            sys_command,
            "export LC_ALL=C\nnmcli d | grep 'wifi\\|CONNECTION'"
        );
    }
    else {
        Luna::ccString::format(
            sys_command,
            "export LC_ALL=C\nnmcli d | grep 'ethernet\\|CONNECTION'"
        );
    }

    std::string result = Luna_system_exec(sys_command);

#if defined(_WIN32) || defined(__APPLE__)
    if (is_wireless_if) {
        result =
            "DEVICE           TYPE      STATE        CONNECTION\n" \
            "wlx0013efc21589  wifi      connected    MarsNetworks\n" \
            "wlan0            wifi      disconnected --\n";
    }
    else {
        result = 
            "DEVICE           TYPE      STATE         CONNECTION\n" \
            "eth0             ethernet  connected     Local Connection\n" \
            "eth1             ethernet  unavailable   --\n";
    }
#endif // _WIN32 || __APPLE__


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
            Luna::ccString found_ifname = line_buffer.substr(item_pos[0], item_pos[1] - 1);
            Luna::ccString type = line_buffer.substr(item_pos[1], (item_pos[2] - 1) - item_pos[1]);
            Luna::ccString state = line_buffer.substr(item_pos[2], (item_pos[3] - 1) - item_pos[2]);
            Luna::ccString connection_name = line_buffer.substr(item_pos[3]);

            found_ifname.trim_right();
            state.trim_right();
            connection_name.trim_right();

            if (ifname == "" || ifname == found_ifname) {
                Json::Value if_info;

                if (connection_name == "--") {
                    Json::Value con_info = look_for_connection_uuid_assciated_ifname(found_ifname);
                    connection_name = con_info["name"].asString();
                }                

                if_info["ifname"] = found_ifname;
                if_info["state"] = state;
                if_info["connection_name"] = connection_name;

                if_list["if_list"].append(if_info);
            }
        }    
    }

    return if_list;
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

Json::Value LunaNetworkConfigManager::get_ipv4_info(const std::string& connection_name) {

    Json::Value if_ipv4_info;

    if_ipv4_info["connection_uuid"] = "";
    if_ipv4_info["method"] = "dhcp";
    if_ipv4_info["address"] = "-";
    if_ipv4_info["subnet_mask"] = "-";
    if_ipv4_info["gateway"] = "-";
    if_ipv4_info["dns"] = "-";

    if (connection_name == "" || connection_name == "--")
        return if_ipv4_info;

    std::string sys_command;

    Luna::ccString::format(
        sys_command,
        "export LC_ALL=C\n"\
        "nmcli c show '%s' | grep 'connection.uuid\\|ipv4.method\\|IP4'",
        connection_name.c_str()
    );
    
    std::string result = Luna_system_exec(sys_command);

#if defined(_WIN32) || defined(__APPLE__)
    result = "" \
        "connection.uuid:                        4d4eed70-3076-4688-aa4c-e0bd10a540d5" \
        "ipv4.method:                            auto\n" \
        "IP4.ADDRESS[1]:                         10.0.1.15/24\n" \
        "IP4.GATEWAY:                            10.0.1.1\n" \
        "IP4.ROUTE[1]:                           dst = 169.254.0.0/16, nh = 0.0.0.0, mt = 1000\n" \
        "IP4.DNS[1]:                             10.0.1.1\n" \
        "IP4.DOMAIN[1]:                          kornet\n";
#endif // _WIN32 || __APPLE__

    std::string find_new_line_pattern = "\n";
    std::string line_buffer;

    std::size_t start_pos = 0;
    std::size_t find_pos = 0;

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

        if (line_buffer.find("connection.uuid") != std::string::npos) {
            auto tokens = split_token<Luna::ccString>(line_buffer, ":");

            if (tokens.size() == 2) {
                tokens[1].trim();
                if_ipv4_info["connection_uuid"] = tokens[1];
            }

            continue;
        } 

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

bool LunaNetworkConfigManager::set_ipv4_config(const std::string& connection_uuid, Json::Value& config) {
    std::cout << "set_ipv4_config, enter" << std::endl;

    if (connection_uuid == "")
        return false;

    std::string sys_command;

    if (config["method"] == "dhcp") {
        Luna::ccString::format(
            sys_command,
            "export LC_ALL=C\n"\
            "nmcli con mod '%s' ipv4.method auto ipv4.addr \"\" ipv4.gateway \"\" ipv4.gateway \"\" ipv4.dns \"\"",
            connection_uuid.c_str()
        ); 
    }
    else {
        int subnet_mask_index = 24;

        if (config["subnet_mask"].asString() == "255.0.0.0")
            subnet_mask_index = 8;
        else if (config["subnet_mask"].asString() == "255.255.0.0")
                subnet_mask_index = 24;

        Luna::ccString::format(
            sys_command,
            "export LC_ALL=C\n"\
            "nmcli con mod '%s' ipv4.method manual ipv4.addr \"%s/%d\" ipv4.gateway \"%s\" ipv4.dns \"%s\"",
            connection_uuid.c_str(),
            config["address"].asString().c_str(),
            subnet_mask_index,
            config["gateway"].asString().c_str(),
            config["dns"].asString().c_str()
        );    
    }

    std::cout << "set_ipv4_config, sys_command: '" << sys_command << "'" << std::endl;

    std::string result = Luna_system_exec(sys_command);

    std::cout << "set_ipv4_config, result: '" << result << "'" << std::endl;

    renew_ipv4_dhcp(connection_uuid);

    return true;
}

bool LunaNetworkConfigManager::renew_ipv4_dhcp(const std::string& connection_uuid) {
    std::string sys_command;
    std::string result;

    Luna::ccString::format(
        sys_command,
        "export LC_ALL=C\n"\
        "nmcli c down '%s'",
        connection_uuid.c_str()
    );
    
    result = Luna_system_exec(sys_command);

    Luna::ccString::format(
        sys_command,
        "export LC_ALL=C\n"\
        "nmcli c up '%s'",
        connection_uuid.c_str()
    );
    
    result = Luna_system_exec(sys_command);

    return true;
}

bool LunaNetworkConfigManager::release_ipv4_dhcp(const std::string& connection_uuid) {
    std::string sys_command;

    Luna::ccString::format(
        sys_command,
        "export LC_ALL=C\n"\
        "nmcli c down '%s'",
        connection_uuid.c_str()
    );
    
    std::string result = Luna_system_exec(sys_command);

    return true;
}


Json::Value LunaNetworkConfigManager::get_wireless_info(const std::string& connection_name) {
    std::string sys_command;

    Luna::ccString::format(
        sys_command,
        "export LC_ALL=C\n"\
        "nmcli c show '%s' | grep 'connection.uuid\\|802-11-wireless.ssid'",
        connection_name.c_str()
    );
    
    std::string result = Luna_system_exec(sys_command);

#if defined(_WIN32) || defined(__APPLE__)
    result = "" \
        "connection.uuid:                        4d4eed70-3076-4688-aa4c-e0bd10a540d5" \
        "802-11-wireless.ssid:                   MarsNetworks\n";
#endif // _WIN32 || __APPLE__

    std::string find_new_line_pattern = "\n";
    std::string line_buffer;

    std::size_t start_pos = 0;
    std::size_t find_pos = 0;

    Json::Value if_wireless_info;

    if_wireless_info["uuid"] = "";
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

        if (line_buffer.find("connection.uuid") != std::string::npos) {
            auto tokens = split_token<Luna::ccString>(line_buffer, ":");

            if (tokens.size() == 2) {
                tokens[1].trim();
                if_wireless_info["uuid"] = tokens[1];
            }

            continue;
        } 

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

/*
    nmcli -f ssid,chan,signal,security d wifi list ifname 'wlx0013efc21589'
    SSID          CHAN  SIGNAL  SECURITY
    iptimejry     1     55      WPA2
    T wifi home   5     55      WPA1 WPA2 802.1X
    SK_WiFiD8A6   5     55      WPA1 WPA2
    MarsNetworks  11    47      WPA2
*/

Json::Value LunaNetworkConfigManager::get_wireless_ssid_list(const std::string& ifname) {
    std::string sys_command;

    Luna::ccString::format(
        sys_command,
        "export LC_ALL=C\n"\
        "nmcli -f ssid,chan,signal,security d wifi list ifname '%s'",
        ifname.c_str()
    );
    
    std::string result = Luna_system_exec(sys_command);

#if defined(_WIN32) || defined(__APPLE__)
    result = "" \
        "SSID          CHAN  SIGNAL  SECURITY\n" \
        "iptimejry     1     55      WPA2\n" \
        "T wifi home   5     55      WPA1 WPA2 802.1X\n" \
        "SK_WiFiD8A6   5     55      WPA1 WPA2\n" \
        "iptimejry2    1     55      WPA2\n" \
        "T wifi home2  5     55      WPA1 WPA2 802.1X\n" \
        "SK_WiFiD8A62  5     55      WPA1 WPA2\n" \
        "iptimejry3    1     55      WPA2\n" \
        "T wifi home3  5     55      WPA1 WPA2 802.1X\n" \
        "SK_WiFiD8A63  5     55      WPA1 WPA2\n" \
        "MarsNetworks  11    47      WPA2\n";

#endif // _WIN32 || __APPLE__

    std::string find_new_line_pattern = "\n";
    std::string find_title_line_pattern = "SECURITY";
    std::string line_buffer;

    std::size_t item_pos[4] = { 0, };

    std::size_t start_pos = 0;
    std::size_t find_pos = 0;

    Json::Value ssid_list;

    int ssid_no = 1;

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

            ssid_info["no"] = ssid_no++;
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
    nmcli c del '4d4eed70-3076-4688-aa4c-e0bd10a540d5'
    Connection 'MarsNetworks' (4d4eed70-3076-4688-aa4c-e0bd10a540d5) successfully deleted.
*/

bool LunaNetworkConfigManager::delete_connection(const std::string& uuid) {
    if (uuid == "")
        return false;

    std::string sys_command;

    Luna::ccString::format(
        sys_command,
        "export LC_ALL=C\n"\
        "nmcli c del '%s'",
        uuid.c_str()
    );
    
    std::string result = Luna_system_exec(sys_command);

 #if defined(_WIN32) || defined(__APPLE__)
    result = "" \
        "Connection 'MarsNetworks' (4d4eed70-3076-4688-aa4c-e0bd10a540d5) successfully deleted.\n";
#endif // _WIN32 || __APPLE__

    if (result.find("successfully deleted") == std::string::npos)
        return false;

    return true;
}

bool LunaNetworkConfigManager::create_wireless_connection(const std::string& ifname, const std::string& ssid, const std::string& password) {
    std::string sys_command;

    Luna::ccString::format(
        sys_command,
        "export LC_ALL=C\n"\
        "sudo nmcli d wifi con '%s' password '%s' ifname '%s'",
        ssid.c_str(),
        password.c_str(),
        ifname.c_str()
    );
    
    std::string result = Luna_system_exec(sys_command);

#if defined(_WIN32) || defined(__APPLE__)
    result = "" \
        "Connection 'MarsNetworks' (4d4eed70-3076-4688-aa4c-e0bd10a540d5) successfully deleted.\n";
#endif // _WIN32 || __APPLE__

    return true;
}

/*
    nmcli -f uuid,type,device c | grep -E '802-11-wireless.*--'

    66d62697-c4ba-4159-9641-157ddeae1a47  802-11-wireless  --              
    1eb2cb55-c56f-4472-acaf-89b747bc3c35  802-11-wireless  --      
*/

bool  LunaNetworkConfigManager::delete_all_zombie_wireless_connection() {
    std::string sys_command;

    Luna::ccString::format(
        sys_command,
        "export LC_ALL=C\n"\
        "nmcli -f uuid,type,device c | grep -E '802-11-wireless.*--'"
    );
    
    std::string result = Luna_system_exec(sys_command);

#if defined(_WIN32) || defined(__APPLE__)
    result = "" \
        "66d62697-c4ba-4159-9641-157ddeae1a47  802-11-wireless  --\n"\
        "1eb2cb55-c56f-4472-acaf-89b747bc3c35  802-11-wireless  --\n";
#endif // _WIN32 || __APPLE__

    std::string find_new_line_pattern = "\n";
    std::string line_buffer;

    std::size_t start_pos = 0;
    std::size_t find_pos = 0;

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

        auto tokens = split_token<Luna::ccString>(line_buffer, "802-11-wireless");

        if (tokens.size() == 2) {
            tokens[0].trim();
            delete_connection(tokens[0]);
        }
    }

    return true;
}
