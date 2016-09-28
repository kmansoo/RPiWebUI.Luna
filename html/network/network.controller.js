(function () {
    'use strict';

    angular
        .module('app')
        .controller('NetworkController', NetworkController);

    NetworkController.$inject = ['$http'];
    function NetworkController($http) {
        var vm = this;

        vm.ip_method = "static";

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
                "title" : "Primary DNS", "id" : "vm.network_primary_dns", "value" : ""                
            },
            {
                "title" : "Status", "id" : "vm.network_status", "value" : ""          
            }            
        ];

        (function initController() {
            for (var index = 0; index < 5; index++)
                vm.network_form[index].value = "-";

            $http.get('/api/network/status')
                .success(function(data, status, headers, config) {
                    if (status == 200) {
                        try {
                            var if_info = data.if_list[0];

                            vm.ip_method = if_info.ipv4.method;

                            vm.network_form[0].value = if_info.ipv4.address;
                            vm.network_form[1].value = if_info.ipv4.subnet_mask;
                            vm.network_form[2].value = if_info.ipv4.gateway;
                            vm.network_form[3].value = if_info.ipv4.dns;

                            if (if_info.state == "connected")
                                vm.network_form[4].value = "Connected";                       
                        } catch (err) {

                        }
                    }
                })
                .error(function(data, status, header, config) {
                });
        })();        
    }
})();
