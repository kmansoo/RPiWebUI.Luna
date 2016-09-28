#ifndef __LUNA_RASPBERRY_PI_WEB_UI_WIRELESS_NETWORK_MANAGER_H__
#define __LUNA_RASPBERRY_PI_WEB_UI_WIRELESS_NETWORK_MANAGER_H__

/*
* LunaWirelessNetworkManager.h
*
*  Created on: 2016. 9. 27.
*      Author: Mansoo Kim(mansoo.kim@icloud.com)
*/

#include "ccJsonParser/json/value.h"

class LunaWirelessNetworkManager {

public:
    LunaWirelessNetworkManager();

public:
    Json::Value get_info(const std::string& finding_ifname = "");

protected:
    Json::Value get_if_list(const std::string& finding_ifname);
    Json::Value get_ssid_list(const std::string& finding_ifname);
    Json::Value get_ipv4_info(const std::string& connection_name);
    Json::Value get_wireless_info(const std::string& connection_name);
};

#endif 