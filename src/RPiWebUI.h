#ifndef __LUNA_RASPBERRY_PI_WEB_UI_RESTFUL_API_H__
#define __LUNA_RASPBERRY_PI_WEB_UI_RESTFUL_API_H__

/*
* RPiWebUI.h
*
*  Created on: 2016. 9. 20.
*      Author: Mansoo Kim(mansoo.kim@icloud.com)
*/

#include "ccWebServer/ccRESTfulApi.h"

class RPiWebUI :
    public Luna::ccRESTfulApi {
public:
    RPiWebUI();

protected:
    bool users(std::shared_ptr<Luna::ccWebServerRequest> pRequest, std::shared_ptr<Luna::ccWebServerResponse> pResponse);
    bool password(std::shared_ptr<Luna::ccWebServerRequest> pRequest, std::shared_ptr<Luna::ccWebServerResponse> pResponse);
    bool rx_tx_power(std::shared_ptr<Luna::ccWebServerRequest> pRequest, std::shared_ptr<Luna::ccWebServerResponse> pResponse);
    bool system_reboot(std::shared_ptr<Luna::ccWebServerRequest> pRequest, std::shared_ptr<Luna::ccWebServerResponse> pResponse);
    bool firmware_upgrade(std::shared_ptr<Luna::ccWebServerRequest> pRequest, std::shared_ptr<Luna::ccWebServerResponse> pResponse);

    bool network_status(std::shared_ptr<Luna::ccWebServerRequest> pRequest, std::shared_ptr<Luna::ccWebServerResponse> pResponse);
    bool wireless_status(std::shared_ptr<Luna::ccWebServerRequest> pRequest, std::shared_ptr<Luna::ccWebServerResponse> pResponse);
};

#endif // !__LUNA_SIMPLE_LOGIN_RESTFUL_API_H__
