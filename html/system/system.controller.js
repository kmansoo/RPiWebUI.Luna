(function () {
    'use strict';

    angular
        .module('app')
        .controller('SystemController', SystemController);

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

    SystemController.$inject = ['$rootScope', 'AuthenticationService', '$http', '$timeout', '$interval', 'ModalService'];
    function SystemController($rootScope, AuthenticationService, $http, $timeout) {

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
                        waitingDialog.show("This device is going to reboot now.\nSo please wait for about 30 seconds while rebooting.");

                        $timeout(function() {
                            waitingDialog.hide();
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

                        waitingDialog.show("This device is going to shut down now.\nSo you must reconnect after turning power on.");

                        $timeout(function() {
                            waitingDialog.hide();
                        }, 5000);
                    }                
                })
                .error(function(data, status, header, config) {

                });
        }        
    }

})();
