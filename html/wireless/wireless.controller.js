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

        vm.wireless_form = [
            {
                "title" : "IPv4", "id" : "vm.nwifi_ipv4", "value" : ""
            },
            {
                "title" : "Subnet Mask", "id" : "vm.wifi_subnetmask", "value" : ""
            },            
            {
                "title" : "Router", "id" : "vm.wifi_router", "value" : ""                
            }
        ];

        (function initController() {
            for (var index = 0; index < 3; index++)
                vm.wireless_form[index].value = "-";

            $http.get('/api/wireless/status')
                .success(function(data, status, headers, config) {
                    if (status == 200) {
                        try {
                            vm.wireless_form[0].value = data["wireless"].ipv4;
                            vm.wireless_form[1].value = data["wireless"].subnet_mask;
                            vm.wireless_form[2].value = data["wireless"].router;
                        } catch (err) {

                        }
                    }
                })
                .error(function(data, status, header, config) {
                });                
        })();
    }

})();
