from django.contrib import admin
from website.models import User,Channel,Sensor,Bridge,Data,Version


admin.site.register(User)
admin.site.register(Channel)
admin.site.register(Sensor)
admin.site.register(Bridge)
admin.site.register(Data)
admin.site.register(Version)
