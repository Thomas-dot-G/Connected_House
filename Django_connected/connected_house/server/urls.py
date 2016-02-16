"""connected_house URL Configuration

The `urlpatterns` list routes URLs to views. For more information please see:
    https://docs.djangoproject.com/en/1.9/topics/http/urls/
Examples:
Function views
    1. Add an import:  from my_app import views
    2. Add a URL to urlpatterns:  url(r'^$', views.home, name='home')
Class-based views
    1. Add an import:  from other_app.views import Home
    2. Add a URL to urlpatterns:  url(r'^$', Home.as_view(), name='home')
Including another URLconf
    1. Import the include() function: from django.conf.urls import url, include
    2. Add a URL to urlpatterns:  url(r'^blog/', include('blog.urls'))
"""
from django.conf.urls import include, url
from django.conf import settings
from django.contrib.staticfiles.urls import staticfiles_urlpatterns
from django.conf.urls.static import static
from django.contrib import admin
import views
admin.autodiscover()


urlpatterns = [
    url(r'^admin/', admin.site.urls),
    url(r'^$', views.index, name='index'),
    url(r'^signin/$', views.signin, name='signin'),
    url(r'^signup/$', views.signup, name='signup'),
    url(r'^dashboard/$', views.dashboard, name='dashboard'),
    url(r'^logout/$', views.logout, name='logout'),
    url(r'^myaccount/$', views.myaccount, name='myaccount'),
    url(r'^myaccount/edit/$', views.myaccount_edit, name='myaccount_edit'),
    url(r'^myaccount/delete/$', views.myaccount_delete, name='myaccount_delete'),
    url(r'^generateapi/$', views.generateapi, name='generateapi'),
    url(r'^mychannels/$', views.mychannels, name='mychannels'),
    url(r'^mychannels/new/$', views.newchannel, name='newchannel'),
    url(r'^mysensors/$', views.sensors, name='sensors'),
    url(r'^mysensors/new/$', views.newsensors, name='newsensors'),
    url(r'^mybridge/new/$', views.newbridge, name='newbridge'),
    url(r'^postdata/$', views.post_data, name='post_data'),
    url(r'^updateme/$', views.post_Version_Bridge, name='post_Version_Bridge'),
    url(r'^updateprobes/$', views.post_Version_Probes, name='post_Version_Probes'),
    url(r'^energy/electricity/$', views.electricity, name='electricity'),
    url(r'^energy/water/$', views.water, name='water'),
    url(r'^energy/photovoltaic/$', views.photovoltaic, name='photovoltaic'),
    url(r'^weather/$', views.weather, name='weather'),
    url(r'^forecast/$', views.forecast, name='forecast'),
    url(r'^advanced/$', views.advanced, name='advanced'),
]+ static(settings.MEDIA_URL, document_root=settings.MEDIA_ROOT)

urlpatterns += staticfiles_urlpatterns()