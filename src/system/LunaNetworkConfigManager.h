#ifndef __LUNA_RASPBERRY_PI_WEB_UI_NETWORK_CONFIG_MANAGER_H__
#define __LUNA_RASPBERRY_PI_WEB_UI_NETWORK_CONFIG_MANAGER_H__

/*
* LunaNetworkConfigManager.h
*
*  Created on: 2016. 9. 27.
*      Author: Mansoo Kim(mansoo.kim@icloud.com)
*/

#include "json/value.h"

class LunaNetworkConfigManager {

public:
    LunaNetworkConfigManager();

public:
    Json::Value get_ethernet_if_list(const std::string& ifname = "");
    Json::Value get_wireless_if_list(const std::string& ifname = "");

    Json::Value get_ipv4_info(const std::string& connection_name);
    bool        set_ipv4_config(const std::string& connection_uuid, Json::Value& config);

    bool        renew_ipv4_dhcp(const std::string& connection_uuid);
    bool        release_ipv4_dhcp(const std::string& connection_uuid);

    Json::Value get_wireless_info(const std::string& connection_name);
    Json::Value get_wireless_ssid_list(const std::string& ifname);

    bool        delete_connection(const std::string& uuid);

    bool        create_wireless_connection(const std::string& ifname, const std::string& ssid, const std::string& password);
    bool        delete_all_zombie_wireless_connection();

private:
    Json::Value get_if_list_impl(bool is_wireless_if, const std::string& ifname);

    std::vector<Luna::ccString> look_for_disconnected_connection_uuid_list();
    Json::Value look_for_connection_uuid_assciated_ifname(const std::string& ifname);

protected:
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
};

#endif 