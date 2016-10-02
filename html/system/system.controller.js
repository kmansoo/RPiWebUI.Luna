(function () {
    'use strict';

    angular
        .module('app')
        .controller('SystemController', SystemController);

    SystemController.$inject = ['$rootScope', 'AuthenticationService', '$http', '$timeout', 'LunaWaitingDialog'];
    function SystemController($rootScope, AuthenticationService, $http, $timeout, LunaWaitingDialog) {

        var vm = this;

        vm.reboot_system = reboot_system;
        vm.shutdown_system = shutdown_system;

        (function initController() {
        })();

        function reboot_system() {
            var api_name = '/api/system/reboot';

            $http.get(api_name)
                .success(function(data, status, headers, config) {
                    if (status == 200) {
                        LunaWaitingDialog.show("This device is going to reboot now.\nSo please wait for about 30 seconds while rebooting.");

                        $timeout(function() {
                            LunaWaitingDialog.hide();
                        }, 30000);
                    }                                     

                })
                .error(function(data, status, header, config) {

                });
        }

        function shutdown_system() {
            var api_name = '/api/system/shutdown';    

            $http.get(api_name)
                .success(function(data, status, headers, config) {
                    if (status == 200) {
                        AuthenticationService.ClearCredentials();

                        LunaWaitingDialog.show("This device is going to shut down now.\nSo you must reconnect after turning power on.");

                        $timeout(function() {
                            LunaWaitingDialog.hide();
                        }, 5000);
                    }                
                })
                .error(function(data, status, header, config) {

                });
        }        
    }

})();
