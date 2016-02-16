from __future__ import unicode_literals

from django.db import models
from timezone_field import TimeZoneField


class User(models.Model):
    name = models.CharField(max_length=200,blank=True)
    API_KEY = models.CharField(max_length=16, null=True)
    email = models.EmailField(max_length=70,blank=True, primary_key=True)
    timezone = TimeZoneField(default='Europe/Paris')
    password = models.CharField(max_length=30)

    def __unicode__(self):
            return self.name

class Bridge(models.Model):
    name = models.CharField(max_length=200, primary_key=True)
    user = models.ForeignKey(User, on_delete=models.CASCADE)
    NETWORKID = models.CharField(max_length=200, null=True)
    NODEID = models.CharField(max_length=200, default='0')
    VERSION = models.CharField(max_length=200, default='V0')

    def __unicode__(self):
        return '%s: %s' % (self.user, self.name)

class Type(models.Model):
    name = models.CharField(max_length=200, primary_key=True)
    
    def __unicode__(self):
            return self.name

class Sensor(models.Model):
    name = models.CharField(max_length=200, primary_key=True)
    user = models.ForeignKey(User, on_delete=models.CASCADE)
    bridge = models.ForeignKey(Bridge, on_delete=models.SET_NULL, null=True) 
    NETWORKID = models.CharField(max_length=200, null=True)
    NODEID = models.CharField(max_length=200, default='0')
    VERSION = models.CharField(max_length=200, default='V0')
    TYPE = models.ForeignKey(Type, on_delete=models.SET_NULL, null=True)
    
    def __unicode__(self):
            return '%s: %s' % (self.user, self.name)

class Channel(models.Model):
    name = models.CharField(max_length=200)
    API_KEY = models.CharField(max_length=16, primary_key=True)
    user = models.ForeignKey(User, on_delete=models.CASCADE)
    sensors = models.ManyToManyField(Sensor)

    def __unicode__(self):

            return '%s/%s' % (self.name, self.user)


class Data(models.Model):
    sensor = models.ForeignKey(Sensor, on_delete=models.CASCADE)
    value = models.IntegerField()
    date = models.DateField(auto_now_add=True, null=True);

    def __unicode__(self):
            return '%s/%s: %d - %s' % (self.sensor.user, self.sensor.TYPE, self.value, self.date)

class Version(models.Model):
    TYPE = models.CharField(max_length=200)
    lastVersion = models.CharField(max_length=200)

    def __unicode__(self):
            return '%s: %s' % (self.TYPE, self.lastVersion)

