(function () {
    'use strict';

    angular
        .module('app')
        .controller('NetworkController', NetworkController);

    NetworkController.$inject = ['$http'];
    function NetworkController($http) {
        var vm = this;

        vm.ip_method = "dhcp";

        vm.network_form = [
            {
                "title" : "IPv4", "id" : "vm.network_ipv4", "value" : ""
            },
            {
                "title" : "Subnet Mask", "id" : "vm.network_subnetmask", "value" : ""
            },            
            {
                "title" : "Router", "id" : "vm.network_router", "value" : ""                
            }
        ];

        (function initController() {
            for (var index = 0; index < 3; index++)
                vm.network_form[index].value = "-";

            $http.get('/api/network/status')
                .success(function(data, status, headers, config) {
                    if (status == 200) {
                        try {
                            vm.network_form[0].value = data.network.ipv4;
                            vm.network_form[1].value = data.network.subnet_mask;
                            vm.network_form[2].value = data.network.router;                     
                        } catch (err) {

                        }
                    }
                })
                .error(function(data, status, header, config) {
                });
        })();        
    }
})();
