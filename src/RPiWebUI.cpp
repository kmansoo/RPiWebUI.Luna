/*
* RPiWebUI.cpp
*
*  Created on: 2016. 9. 20.
*      Author: Mansoo Kim(mansoo.kim@icloud.com)
*/

#include "RPiWebUI.h"

#include "ccCore/ccString.h"

#include "json/value.h"
#include "json/reader.h"
#include "json/writer.h"

#include "md5/md5.h"

#include "system/LunaSystemCall.h"
#include "system/LunaNetworkConfigManager.h"

//  I'll implement a user manager in next version. 
//  So this version has just supported like simple functions below for supporting user info.
const char* luna_getLoginID() {
    return "root";
}

const char* luna_getLoginPassword() {
    return "admin";
}

RPiWebUI::RPiWebUI() {
    addAPI(std::string("/api/users/*"), std::bind(&RPiWebUI::users, this, std::placeholders::_1, std::placeholders::_2));

    addAPI(std::string("/api/network/status"), std::bind(&RPiWebUI::network_status, this, std::placeholders::_1, std::placeholders::_2));

    addAPI(std::string("/api/network/connection/ipv4/dhcp"), std::bind(&RPiWebUI::network_connection_ipv4_dhcp, this, std::placeholders::_1, std::placeholders::_2));

    addAPI(std::string("/api/wireless/status"), std::bind(&RPiWebUI::wireless_status, this, std::placeholders::_1, std::placeholders::_2));
    addAPI(std::string("/api/wireless/ssid/*"), std::bind(&RPiWebUI::wireless_ssid, this, std::placeholders::_1, std::placeholders::_2));

    addAPI(std::string("/api/system/reboot"), std::bind(&RPiWebUI::system_reboot, this, std::placeholders::_1, std::placeholders::_2));
    addAPI(std::string("/api/system/shutdown"), std::bind(&RPiWebUI::system_shutdown, this, std::placeholders::_1, std::placeholders::_2));
}

RPiWebUI::~RPiWebUI() {
    if (system_thread_) {
        system_thread_->join();
        system_thread_ = nullptr;
    }
}

bool RPiWebUI::users(std::shared_ptr<Luna::ccWebServerRequest> pRequest, std::shared_ptr<Luna::ccWebServerResponse> pResponse) {
    switch ((std::uint32_t)pRequest->get_method()) {
    case Luna::ccWebServerRequest::HttpMethod_Get:
    {
        //  I'll implement this method as soon as possible.
        std::string user_id = pRequest->get_resource();

        if (user_id == luna_getLoginID()) {
            Json::Value response_json_value;
            Json::StyledWriter json_writer;

            response_json_value["user"] = user_id;
            response_json_value["password"] = BZF::md5(luna_getLoginPassword());

            std::string json_data = json_writer.write(response_json_value);

            pResponse->send_status(200, std::string("OK"));
            pResponse->send_content_type("application/javascript", json_data.length());
            pResponse->send_content(json_data);
        } else
            pResponse->send_status(404, std::string("not found."), true);

        break;
    }
    }

    return true;
}

//  for network
bool RPiWebUI::network_status(std::shared_ptr<Luna::ccWebServerRequest> pRequest, std::shared_ptr<Luna::ccWebServerResponse> pResponse) {
    switch ((std::uint32_t)pRequest->get_method()) {
    case Luna::ccWebServerRequest::HttpMethod_Get:
    {
        Json::StyledWriter json_writer;
        LunaNetworkConfigManager ncm;

        std::string json_data = json_writer.write(ncm.get_ethernet_if_list());
        
        pResponse->send_status(200, std::string("OK"));
        pResponse->send_content_type("application/javascript", json_data.length());
        pResponse->send_content(json_data);
        break;
    }
    }

    return true;
}


bool RPiWebUI::network_connection_ipv4_dhcp(std::shared_ptr<Luna::ccWebServerRequest> pRequest, std::shared_ptr<Luna::ccWebServerResponse> pResponse) {
    switch ((std::uint32_t)pRequest->get_method()) {
    case Luna::ccWebServerRequest::HttpMethod_Put:
    {
        std::string data;

        pRequest->get_content_body(data);

        Json::Reader    json_reader;
        Json::Value     root_value;

        if (!json_reader.parse(data, root_value)) {
            pResponse->send_status(400, std::string("400 Bad Request."), true);
            return false;
        }

        LunaNetworkConfigManager ncm;

        if (root_value["operation"].asString() == "renew")
            ncm.renew_ipv4_dhcp(root_value["connection_uuid"].asString());

        if (root_value["operation"].asString() == "release")
            ncm.release_ipv4_dhcp(root_value["connection_uuid"].asString());

        if (root_value["operation"].asString() == "change_ip_config")
            ncm.set_ipv4_config(root_value["connection_uuid"].asString(), root_value["config"]);  

        pResponse->send_status(200, std::string("OK"), true);
        break;
    }
    }

    return true;
}

//  for wireless
bool RPiWebUI::wireless_status(std::shared_ptr<Luna::ccWebServerRequest> pRequest, std::shared_ptr<Luna::ccWebServerResponse> pResponse) {
    switch ((std::uint32_t)pRequest->get_method()) {
    case Luna::ccWebServerRequest::HttpMethod_Get:
    {
        Json::StyledWriter json_writer;
        LunaNetworkConfigManager ncm;

        std::string json_data = json_writer.write(ncm.get_wireless_if_list());

        pResponse->send_status(200, std::string("OK"));
        pResponse->send_content_type("application/javascript", json_data.length());
        pResponse->send_content(json_data);
        break;
    }
    }

    return true;
}

//  for wireless
bool RPiWebUI::wireless_ssid(std::shared_ptr<Luna::ccWebServerRequest> pRequest, std::shared_ptr<Luna::ccWebServerResponse> pResponse) {
    switch ((std::uint32_t)pRequest->get_method()) {
    case Luna::ccWebServerRequest::HttpMethod_Get: {
        std::string ifname = pRequest->get_resource();
        
        Json::StyledWriter json_writer;
        LunaNetworkConfigManager ncm;

        std::string json_data = json_writer.write(ncm.get_wireless_ssid_list(ifname));

        pResponse->send_status(200, std::string("OK"));
        pResponse->send_content_type("application/javascript", json_data.length());
        pResponse->send_content(json_data);
        break;
    }

    case Luna::ccWebServerRequest::HttpMethod_Post: {
        std::string data;
        std::string ssid = pRequest->get_resource();

        pRequest->get_content_body(data);

        Json::Reader    json_reader;
        Json::Value     root_value;

        if (!json_reader.parse(data, root_value)) {
            pResponse->send_status(400, std::string("400 Bad Request."), true);
            return false;
        }

        LunaNetworkConfigManager ncm;

        ncm.delete_connection(root_value["current_connection_uuid"].asString());
        ncm.delete_all_zombie_wireless_connection();

        ncm.create_wireless_connection(root_value["ifname"].asString(), ssid, root_value["password"].asString());

        pResponse->send_status(200, std::string("OK"), true);
        break;
    }
    }

    return true;
}

bool RPiWebUI::system_reboot(std::shared_ptr<Luna::ccWebServerRequest> pRequest, std::shared_ptr<Luna::ccWebServerResponse> pResponse) {
    switch ((std::uint32_t)pRequest->get_method()) {
    case Luna::ccWebServerRequest::HttpMethod_Get:
    {  
        pResponse->send_status(200, std::string("OK"), true);

        if (system_thread_) {
            system_thread_->join();
            system_thread_ = nullptr;
        }

        system_thread_ = std::make_shared<std::thread>([]() {
            std::this_thread::sleep_for(std::chrono::seconds(2));
            Luna_system_exec("reboot");
        });

        break;
    }
    }

    return true;
}

bool RPiWebUI::system_shutdown(std::shared_ptr<Luna::ccWebServerRequest> pRequest, std::shared_ptr<Luna::ccWebServerResponse> pResponse) {
    switch ((std::uint32_t)pRequest->get_method()) {
    case Luna::ccWebServerRequest::HttpMethod_Get:
    {
        pResponse->send_status(200, std::string("OK"), true);

        if (system_thread_) {
            system_thread_->join();
            system_thread_ = nullptr;
        }

        system_thread_ = std::make_shared<std::thread>([]() {
            std::this_thread::sleep_for(std::chrono::seconds(2));
            Luna_system_exec("shutdown now");
        });

        break;
    }
    }

    return true;
}
