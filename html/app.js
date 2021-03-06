(function () {
    'use strict';

    angular
        .module('app', ['ngRoute', 'ngCookies', 'angular-md5', 'ngFileUpload', 'angularModalService', 'ui.bootstrap'])
        .config(config)
        .run(run);

    config.$inject = ['$routeProvider', '$locationProvider'];
    function config($routeProvider, $locationProvider) {
        $routeProvider
            .when('/', {
                controller: 'StatusController',
                templateUrl: 'status/status.view.html',
                controllerAs: 'vm'
            })

            .when('/network', {
                controller: 'NetworkController',
                templateUrl: 'network/network.view.html',
                controllerAs: 'vm'
            })

             .when('/wireless', {
                controller: 'WirelessController',
                templateUrl: 'wireless/wireless.view.html',
                controllerAs: 'vm'
            })

            .when('/system', {
                controller: 'SystemController',
                templateUrl: 'system/system.view.html',
                controllerAs: 'vm'
            })       

            .when('/login', {
                controller: 'LoginController',
                templateUrl: 'login/login.view.html',
                controllerAs: 'vm'
            })

            .otherwise({ redirectTo: '/' });
    }

    run.$inject = ['$rootScope', '$location', '$cookieStore', '$http'];
    function run($rootScope, $location, $cookieStore, $http) {
        // keep user logged in after page refresh
        $rootScope.globals = $cookieStore.get('globals') || {};
        if ($rootScope.globals.currentUser) {
            $http.defaults.headers.common['Authorization'] = 'Basic ' + $rootScope.globals.currentUser.authdata; // jshint ignore:line
        }

        $rootScope.$on('$locationChangeStart', function (event, next, current) {
            // redirect to login page if not logged in and trying to access a restricted page
            var restrictedPage = $.inArray($location.path(), ['/login']) === -1;
            var loggedIn = $rootScope.globals.currentUser;
            if (restrictedPage && !loggedIn) {
                $location.path('/login');
            }
        });
    }
})();
