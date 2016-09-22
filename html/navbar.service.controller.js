(function () {
    'use strict';

    angular
        .module('app')
        .controller('NavBarServiceController', NavBarServiceController);

    NavBarServiceController.$inject = ['$rootScope', '$scope', '$location', 'AuthenticationService'];
    function NavBarServiceController($rootScope, $scope, $location, AuthenticationService) {

        $scope.is_loged_in = is_loged_in;
        $scope.login_title = login_title;
        $scope.login_icon_type = login_icon_type;
        $scope.logout_if_status_is_logedin = logout_if_status_is_logedin;

        function is_loged_in() {       
            try {
                var loggedIn = $rootScope.globals.currentUser;

                if (loggedIn)
                    return true;

            } catch (err) {         
            }

            return false;
        }

        function login_title() {
            if (is_loged_in())
                return " Logout";
            
            return " Login";
        }

        function login_icon_type() {
            if (is_loged_in())
                return "glyphicon-log-out";
            
            return "glyphicon-log-in";
        }        

        function logout_if_status_is_logedin() {
            if (is_loged_in()) {
                // reset login status
                AuthenticationService.ClearCredentials();

                $location.path('/login');
            }
        }
     }
})();
