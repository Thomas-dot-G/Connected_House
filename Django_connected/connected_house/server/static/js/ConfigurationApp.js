var ConfigurationApp = angular.module('ConfigurationApp', ['ngRoute', 'angularLocalStorage', 'ui.bootstrap']);

ConfigurationApp.config(['$routeProvider',
  function($routeProvider) {
    $routeProvider.
		when('/', {
			templateUrl: '/pages/configuration/index.html',
			controller: 'ConfigurationController',
		}).
		when('/new-channel', {
			templateUrl: '/pages/configuration/new-channel.html',
			controller: 'NewChannelController',
		}).
		/*when('/edit-channel', {
			templateUrl: '/pages/configuration/edit-channel.html',
			controller: 'EditChannelController',
		}).*/
		otherwise({
			redirectTo: '/'
		});
  	}
]);

