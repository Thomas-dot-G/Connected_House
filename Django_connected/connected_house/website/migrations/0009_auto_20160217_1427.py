# -*- coding: utf-8 -*-
# Generated by Django 1.9.1 on 2016-02-17 13:27
from __future__ import unicode_literals

from django.db import migrations, models


class Migration(migrations.Migration):

    dependencies = [
        ('website', '0008_user_prefered_channel'),
    ]

    operations = [
        migrations.AlterField(
            model_name='user',
            name='prefered_channel',
            field=models.CharField(max_length=200, null=True),
        ),
    ]
