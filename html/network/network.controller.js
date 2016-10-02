(function () {
    'use strict';

    angular
        .module('app')
        .controller('NetworkController', NetworkController);

    NetworkController.$inject = ['$http', '$timeout', 'LunaWaitingDialog'];
    function NetworkController($http, $timeout, LunaWaitingDialog) {
        var vm = this;

        vm.scaned_if_list = [];
        vm.selected_ifname = "";
        vm.selected_if_info = [];

        vm.ifname_change = ifname_change;
        vm.dhcp_renew = ipv4_dhcp_renew;
        vm.dhcp_release = ipv4_dhcp_release;
        vm.save_configuration = set_ipv4_configuration;

        vm.ip_method = "static";
        vm.connection_status = "Disconnected";
        vm.connection_uuid =  "";
        vm.ipv4_address = "";
        vm.ipv4_subnet_mask = "";
        vm.ipv4_gateway = "";
        vm.ipv4_dns = "";

        (function initController() {
            get_state();
        })();


        function get_state() {
            vm.ipv4_address = "";
            vm.ipv4_subnet_mask = "";
            vm.ipv4_gateway = "";
            vm.ipv4_dns = "";
                                    
            $http.get('/api/network/status')
                .success(function(data, status, headers, config) {
                    if (status == 200) {
                        try {
                            vm.scaned_if_list = data.if_list;

                            for (var index in vm.scaned_if_list) {
                                var item = vm.scaned_if_list[index];

                                if (item.state == "connected") {
                                    update_state_after_changing_ifname(item);
                                    return;
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

            vm.ip_method = vm.selected_if_info.ipv4.method;
            vm.connection_uuid = "";
        
            vm.ipv4_address  = vm.selected_if_info.ipv4.address;
            vm.ipv4_subnet_mask  = vm.selected_if_info.ipv4.subnet_mask;
            vm.ipv4_gateway  = vm.selected_if_info.ipv4.gateway;
            vm.ipv4_dns  = vm.selected_if_info.ipv4.dns;

            if (vm.selected_if_info.state == "connected")
                vm.connection_status= "Connected";
            else
                vm.connection_status = "Disconnected";

            vm.connection_uuid =  vm.selected_if_info.ipv4.connection_uuid; 
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
                "operation" : 'renew'
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
                "operation" : 'release'
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
