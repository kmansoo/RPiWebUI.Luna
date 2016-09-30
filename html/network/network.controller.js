(function () {
    'use strict';

    angular
        .module('app')
        .controller('NetworkController', NetworkController);

    var waitingDialog = waitingDialog || (function ($) {
        'use strict';

        // Creating modal dialog's DOM
        var $dialog = $(
            '<div class="modal fade" data-backdrop="static" data-keyboard="false" tabindex="-1" role="dialog" aria-hidden="true" style="padding-top:15%; overflow-y:visible;">' +
            '<div class="modal-dialog modal-m">' +
            '<div class="modal-content">' +
            '<div class="modal-header"><h3 style="margin:0;"></h3></div>' +
            '<div class="modal-body">' +
            '<div class="progress progress-striped active" style="margin-bottom:0;"><div class="progress-bar" style="width: 100%"></div></div>' +
            '</div>' +
            '</div></div></div>');

        return {
            /**
             * Opens our dialog
             * @param message Custom message
             * @param options Custom options:
             * 				  options.dialogSize - bootstrap postfix for dialog size, e.g. "sm", "m";
             * 				  options.progressType - bootstrap postfix for progress bar type, e.g. "success", "warning".
             */
            show: function (message, options) {
                // Assigning defaults
                if (typeof options === 'undefined') {
                    options = {};
                }
                if (typeof message === 'undefined') {
                    message = 'Loading';
                }
                var settings = $.extend({
                    dialogSize: 'm',
                    progressType: '',
                    onHide: null // This callback runs after the dialog was hidden
                }, options);

                // Configuring dialog
                $dialog.find('.modal-dialog').attr('class', 'modal-dialog').addClass('modal-' + settings.dialogSize);
                $dialog.find('.progress-bar').attr('class', 'progress-bar');
                if (settings.progressType) {
                    $dialog.find('.progress-bar').addClass('progress-bar-' + settings.progressType);
                }
                $dialog.find('h3').text(message);
                // Adding callbacks
                if (typeof settings.onHide === 'function') {
                    $dialog.off('hidden.bs.modal').on('hidden.bs.modal', function (e) {
                        settings.onHide.call($dialog);
                    });
                }
                // Opening dialog
                $dialog.modal();
            },
            /**
             * Closes dialog
             */
            hide: function () {
                $dialog.modal('hide');
            }
        };

    })(jQuery);

    NetworkController.$inject = ['$http', '$timeout'];
    function NetworkController($http, $timeout) {
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
            waitingDialog.show("Please wait while configuring DHCP renew.");

            $timeout(function() {
                get_state();
                waitingDialog.hide();
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
            waitingDialog.show("Please wait while configuring DHCP release.");

            $timeout(function() {
                get_state();
                waitingDialog.hide();
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
            waitingDialog.show("Please wait while configuring IPv4");

            $timeout(function() {
                get_state();
                waitingDialog.hide();
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
