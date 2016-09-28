(function () {
    'use strict';

    angular
        .module('app')
        .controller('WirelessController', WirelessController);

    WirelessController.$inject = ['$scope', '$http'];
    function WirelessController($scope, $http) {
        $scope.message = true;  //  It can enable what the ng-hide work.

        var vm = this;

        vm.ip_method = "dhcp";
        vm.get_ssid = get_ssid;

        vm.wireless_form = [
            {
                "title" : "SSID", "id" : "vm.wifi_ssid", "value" : ""
            },            
            {
                "title" : "IPv4", "id" : "vm.network_ipv4", "value" : ""
            },
            {
                "title" : "Subnet Mask", "id" : "vm.network_subnetmask", "value" : ""
            },            
            {
                "title" : "Router", "id" : "vm.network_router", "value" : ""                
            },
            {
                "title" : "Primary DNS", "id" : "vm.network_primary_dns", "value" : ""                
            },
            {
                "title" : "Status", "id" : "vm.network_status", "value" : ""          
            }   
        ];

        vm.scaned_ssid_list = [
        ];           

        (function initController() {
            for (var index = 0; index < 6; index++)
                vm.wireless_form[index].value = "-";

            $http.get('/api/wireless/status')
                .success(function(data, status, headers, config) {
                    if (status == 200) {
                        try {
                            var if_info = data.if_list[0];

                            vm.wireless_form[0].value = if_info.wireless.ssid;
                            vm.wireless_form[1].value = if_info.ipv4.address;
                            vm.wireless_form[2].value = if_info.ipv4.subnet_mask;
                            vm.wireless_form[3].value = if_info.ipv4.gateway;
                            vm.wireless_form[4].value = if_info.ipv4.dns;

                            if (if_info.state == "connected")
                                vm.wireless_form[5].value = "Connected";                            
                        } catch (err) {

                        }
                    }
                })
                .error(function(data, status, header, config) {
                });                
        })();

        function get_ssid() {

            $http.get('/api/wireless/ssid')
                .success(function(data, status, headers, config) {
                    if (status == 200) {
                        vm.scaned_ssid_list = [];

                        if (data.if_list) {
                            for (var if_index = 0; if_index < data.if_list.length; if_index++) {
                                var if_info = data.if_list[if_index];
                                var ssid_list = if_info.ssid_list;

                                if (ssid_list) {
                                    for (var ssid_index = 1; ssid_index <= ssid_list.length; ssid_index++) {
                                        var ssid_item = ssid_list[ssid_index - 1];
                                        var ssid_info = [];

                                        ssid_info.no = ssid_index.toString();
                                        ssid_info.ssid = ssid_item.ssid;
                                        ssid_info.channel = ssid_item.channel;
                                        ssid_info.signal = ssid_item.signal;
                                        ssid_info.security = ssid_item.security;

                                        vm.scaned_ssid_list.push(ssid_info);
                                    }
                                }
                            }
                        }


                        console.log(data);
                    }
                    else
                        handleError("Couldn't get the password! : " + status);
                })
                .error(function(data, status, header, config) {
                    handleError("Couldn't get the password! : " + status);
                    
                });

        }
        
    }

})();
