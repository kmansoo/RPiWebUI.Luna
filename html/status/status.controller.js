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

        vm.wifi_network_form = [
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

        /*
        vm.network_form[0].value = "10.0.1.12";
        vm.network_form[1].value = "255.255.255.0";
        vm.network_form[2].value = "10.0.1.1";
        vm.network_form[3].value = "Disconnected";
        */

        vm.wifi_network_form[0].value = "-";
        vm.wifi_network_form[1].value = "-";
        vm.wifi_network_form[2].value = "-";
        vm.wifi_network_form[3].value = "-";
        vm.wifi_network_form[4].value = "-";

        (function initController() {
            for (var index = 0; index < 4; index++)
                vm.network_form[index].value = "-";

            for (var index = 0; index < 4; index++)
                vm.wifi_network_form[index].value = "-";

            vm.network_form[3].value = "Disconnected";
            vm.wifi_network_form[4].value = "Disconnected";

            $http.get('/api/network/status')
                .success(function(data, status, headers, config) {
                    if (status == 200) {
                        try {
                            vm.network_form[0].value = data.network.ipv4;
                            vm.network_form[1].value = data.network.subnet_mask;
                            vm.network_form[2].value = data.network.router;

                            if (data.network.is_connected == true)
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
                            vm.wifi_network_form[0].value = data["wireless"].ssid;
                            vm.wifi_network_form[1].value = data["wireless"].ipv4;
                            vm.wifi_network_form[2].value = data["wireless"].subnet_mask;
                            vm.wifi_network_form[3].value = data["wireless"].router;

                            if (data["wireless"].is_connected == true)
                                vm.wifi_network_form[4].value = "Connected";
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
