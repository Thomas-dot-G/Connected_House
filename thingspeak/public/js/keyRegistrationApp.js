var keyRegistrationApp = angular.module('keyRegistrationApp', ['ngRoute', 'angularLocalStorage', 'ui.bootstrap']);

keyRegistrationApp.config(['$routeProvider',
  function($routeProvider) {
    $routeProvider.
    	when('/', {
    		templateUrl: '/pages/configuration/key_registrations.html',
			controller: 'KeyRegistrationController',
    	}).
		otherwise({
			redirectTo: '/'
		});
}
]);