(function () {
    'use strict';

    angular
        .module('app')
        .controller('WirelessController', WirelessController);

    WirelessController.$inject = ['$scope', '$http', '$timeout', "LunaWaitingDialog"];
    function WirelessController($scope, $http, $timeout, LunaWaitingDialog) {
        $scope.message = true;  //  It can enable what the ng-hide work.

        var vm = this;

        vm.scaned_if_list = [];
        vm.scaned_ssid_list = [];
        
        vm.selected_ifname = "";
        vm.selected_if_info = [];        
        vm.selected_ssid = "";
        vm.selected_ssid_password = "";

        vm.is_connected = false;

        vm.ip_method = "dhcp";
        vm.connection_uuid = "";

        vm.connected_ssid = "";
        vm.ipv4_address = "";
        vm.ipv4_subnet_mask = "";
        vm.ipv4_gateway = "";
        vm.ipv4_dns = "";

        vm.get_ssid = get_ssid;
        vm.connect_to_new_ssid = connect_to_new_ssid;
        vm.ifname_change = ifname_change;
        vm.dhcp_renew = ipv4_dhcp_renew;
        vm.dhcp_release = ipv4_dhcp_release;
        vm.save_configuration = set_ipv4_configuration;

        (function initController() {
            get_state();
        })();

        function get_state() {
            vm.connected_ssid = "";
            vm.ipv4_address = "";
            vm.ipv4_subnet_mask = "";
            vm.ipv4_gateway = "";
            vm.ipv4_dns = "";

            $http.get('/api/wireless/status')
                .success(function(data, status, headers, config) {
                    if (status == 200) {
                        try {
                            vm.scaned_if_list = data.if_list;
                            vm.is_connected = false;

                            for (var index in vm.scaned_if_list) {
                                var item = vm.scaned_if_list[index];

                                if (vm.is_connected == false && item.state == "connected") {
                                    update_state_after_changing_ifname(item);
                                    break;
                                }
                            }

                            if (vm.is_connected == false && vm.scaned_if_list.length > 0) {
                                vm.selected_if_info = vm.scaned_if_list[0];        
                                vm.selected_ifname = vm.selected_if_info.ifname;
                            }

                        } catch (err) {

                        }
                    }
                })
                .error(function(data, status, header, config) {
                });            
        }

        function ifname_change() {
            for (var index in vm.scaned_if_list) {
                var item = vm.scaned_if_list[index];

                if (item.ifname == vm.selected_ifname) {
                    update_state_after_changing_ifname(item);
                    return;
                }
            }        
        }

        function update_state_after_changing_ifname(if_info) {
            vm.selected_if_info = if_info;

            vm.selected_ifname = vm.selected_if_info.ifname;
            vm.connection_uuid = "";

            if (if_info.state == "connected") {
                vm.ip_method = if_info.ipv4.method;

                vm.connected_ssid = if_info.wireless.ssid;
                vm.ipv4_address = if_info.ipv4.address;
                vm.ipv4_subnet_mask = if_info.ipv4.subnet_mask;
                vm.ipv4_gateway = if_info.ipv4.gateway;
                vm.ipv4_dns = if_info.ipv4.dns;
                
                vm.connection_uuid = if_info.ipv4.connection_uuid;                         
                vm.is_connected = true;
            }
            else
                vm.is_connected = false;
        }           

        function get_ssid() {
            var api = '/api/wireless/ssid/' + vm.selected_ifname;

            $http.get(api)
                .success(function(data, status, headers, config) {
                    if (status == 200) {
                        vm.scaned_ssid_list = data;
                    }
                    else
                        handleError("Couldn't get the password! : " + status);
                })
                .error(function(data, status, header, config) {
                    handleError("Couldn't get the password! : " + status);
                    
                });
        }

        //  Method      Description      
        //  POST        Create
        //  GET         Select    
        //  PUT         Update    
        //  DELETE      DELETE    
        function connect_to_new_ssid() {
            var api = '/api/wireless/ssid/' + vm.selected_ssid;
            var data = {
                "ifname" : vm.selected_ifname,
                "password" : vm.selected_ssid_password,
                "current_connection_uuid" : vm.selected_if_info.wireless.uuid,
            };
            
            vm.scaned_ssid_list = [];
            vm.selected_ssid = "";
            vm.selected_ssid_password = "";

            LunaWaitingDialog.show("Please wait while configuring wireless.");

            $timeout(function() {
                LunaWaitingDialog.hide();
                get_state();
            }, 4000);
            
            $http.post(api, data)
                .success(function(data, status, headers, config) {
                    if (status == 200) {
                    }
                    else
                        handleError("Couldn't get the password! : " + status);
                })
                .error(function(data, status, header, config) {
                    handleError("Couldn't get the password! : " + status);
                });                
        }        

        function ipv4_dhcp_renew() {
            LunaWaitingDialog.show("Please wait while configuring DHCP renew.");

            $timeout(function() {
                get_state();
                LunaWaitingDialog.hide();
            }, 2000);

            var api = '/api/network/connection/ipv4/dhcp';
            var data = {
                "connection_uuid" : vm.selected_if_info.ipv4.connection_uuid,
                "method" : 'renew'
            };

            $http.put(api, data)
                .success(function(data, status, headers, config) {
                    if (status == 200) {
                    }
                    else
                        handleError("Couldn't get the password! : " + status);
                })
                .error(function(data, status, header, config) {
                    handleError("Couldn't get the password! : " + status);
                });
        }

        function ipv4_dhcp_release() {
            LunaWaitingDialog.show("Please wait while configuring DHCP release.");

            $timeout(function() {
                get_state();
                LunaWaitingDialog.hide();
            }, 2000);

            var api = '/api/network/connection/ipv4/dhcp';
            var data = {
                "connection_uuid" : vm.selected_if_info.ipv4.connection_uuid,
                "method" : 'release'
            };

            $http.put(api, data)
                .success(function(data, status, headers, config) {
                    if (status == 200) {
                    }
                    else
                        handleError("Couldn't get the password! : " + status);
                })
                .error(function(data, status, header, config) {
                    handleError("Couldn't get the password! : " + status);
                });            
        }
        
        function set_ipv4_configuration() {
            LunaWaitingDialog.show("Please wait while configuring IPv4");

            $timeout(function() {
                get_state();
                LunaWaitingDialog.hide();
            }, 4000);

            var api = '/api/network/connection/ipv4/dhcp';
            var data = {
                "connection_uuid" : vm.selected_if_info.ipv4.connection_uuid,
                "operation" : 'change_ip_config',
                "config" : {
                    "ifname" : vm.selected_ifname,
                    "method" : vm.ip_method,
                    "address": vm.ipv4_address,
                    "subnet_mask": vm.ipv4_subnet_mask,
                    "gateway": vm.ipv4_gateway,
                    "dns": vm.ipv4_dns,
                }
            };

            $http.put(api, data)
                .success(function(data, status, headers, config) {
                    if (status == 200) {
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
