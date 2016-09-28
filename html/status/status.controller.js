(function () {
    'use strict';

    angular
        .module('app')
        .controller('StatusController', StatusController);

    StatusController.$inject = ['$scope', '$rootScope', 'AuthenticationService', '$http', '$timeout', '$interval', 'ModalService'];
    function StatusController($scope, $rootScope, AuthenticationService, $http, $timeout, $interval, ModalService) {

        $scope.message = true;  //  It can enable what the ng-hide work.
        
        var vm = this;

        vm.network_form = [
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
                "title" : "Status", "id" : "vm.network_status", "value" : ""          
            }
        ];

        vm.wireless_form = [
            {
                "title" : "SSID", "id" : "vm.wifi_ssid", "value" : ""
            },
            {
                "title" : "IPv4", "id" : "vm.nwifi_ipv4", "value" : ""
            },
            {
                "title" : "Subnet Mask", "id" : "vm.wifi_subnetmask", "value" : ""
            },            
            {
                "title" : "Router", "id" : "vm.wifi_router", "value" : ""                
            },  
            {
                "title" : "Status", "id" : "vm.wifi_status", "value" : ""            
            }
        ];        

        vm.has_wifi_interface = has_wifi_interface;

        (function initController() {
            for (var index = 0; index < 4; index++)
                vm.network_form[index].value = "-";

            for (var index = 0; index < 5; index++)
                vm.wireless_form[index].value = "-";

            vm.network_form[3].value = "Disconnected";
            vm.wireless_form[4].value = "Disconnected";

            $http.get('/api/network/status')
                .success(function(data, status, headers, config) {
                    if (status == 200) {
                        try {
                            var if_info = data.if_list[0];

                            vm.network_form[0].value = if_info.ipv4.address;
                            vm.network_form[1].value = if_info.ipv4.subnet_mask;
                            vm.network_form[2].value = if_info.ipv4.gateway;

                            if (if_info.state == "connected")
                                vm.network_form[3].value = "Connected";                     
                        } catch (err) {

                        }
                    }
                })
                .error(function(data, status, header, config) {
                });

            $http.get('/api/wireless/status')
                .success(function(data, status, headers, config) {
                    if (status == 200) {
                        try {
                            var if_info = data.if_list[0];

                            vm.wireless_form[0].value = if_info.wireless.ssid;
                            vm.wireless_form[1].value = if_info.ipv4.address;
                            vm.wireless_form[2].value = if_info.ipv4.subnet_mask;
                            vm.wireless_form[3].value = if_info.ipv4.gateway;

                            if (if_info.state == "connected")
                                vm.wireless_form[4].value = "Connected";
                        } catch (err) {

                        }
                    }
                })
                .error(function(data, status, header, config) {
                });                

            /*
            var check_system = $interval(function() {
                var result = Math.floor(Math.random() * 10);

                if ((result % 2) == 0) {
                    vm.network_form[0].value = "-";
                    vm.network_form[1].value = "-";
                    vm.network_form[2].value = "-";
                    vm.network_form[3].value = "Disconnected";                    
                } else {
                    vm.network_form[0].value = "10.0.1.10";
                    vm.network_form[1].value = "255.255.255.0";
                    vm.network_form[2].value = "10.0.1.1";
                    vm.network_form[3].value = "Connected";                   
                }

            }, 3000);
            */
        })();

        function has_wifi_interface() {
            var result = Math.floor(Math.random() * 10);

            if ((result % 2) == 0)
                return false;

            return true;
        }
    }

})();
