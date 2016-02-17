from django import forms
from django.conf import settings
from django.contrib.auth import authenticate
from django.contrib.auth.forms import AuthenticationForm, ReadOnlyPasswordHashField

from website.models import User, Data, Sensor, Bridge, Channel


class SignInForm(forms.ModelForm):

    remember = forms.BooleanField(label='remember', widget=forms.CheckboxInput)
    
    class Meta:
        model = User
        widgets = {
        'password': forms.PasswordInput(),
        }
        fields = ('email', 'password')


    def __init__(self, request=None, *args, **kwargs):
        super(SignInForm, self).__init__(*args, **kwargs)
        for f in ['email', 'password']:
            self.fields[f].widget.attrs.update({'required': 'required'})
            self.fields[f].required = True
        self.fields['remember'].required = False

    def clean(self):
        self.cleaned_data


class SignUpForm(forms.ModelForm):

    password2 = forms.CharField(label='Confirm password', widget=forms.PasswordInput)
    
    class Meta:
        model = User
        widgets = {
        'password': forms.PasswordInput(),
        }
        fields = ('name', 'email', 'timezone', 'password')


    def __init__(self, request=None, *args, **kwargs):
        super(SignUpForm, self).__init__(*args, **kwargs)
        for f in ['email', 'password']:
            self.fields[f].widget.attrs.update({'required': 'required'})
            self.fields[f].required = True


    def clean(self):
        if self.data['password'] == self.data['password2']:
            return self.cleaned_data
        else:
            raise forms.ValidationError('Passwords do not match')

class EditAccountForm(forms.ModelForm):

    password2 = forms.CharField(label='Confirm password', widget=forms.PasswordInput)
    
    class Meta:
        model = User
        widgets = {
        'password': forms.PasswordInput(),
        }
        fields = ('name', 'timezone', 'password')


    def __init__(self, request=None, *args, **kwargs):
        super(EditAccountForm, self).__init__(*args, **kwargs)
        for f in ['timezone', 'name']:
            self.fields[f].widget.attrs.update({'required': 'required'})
            self.fields[f].required = True
        for f in ['password']:
            self.fields[f].widget.attrs.update({'required': 'required'})
            self.fields[f].required = False


    def clean(self):
        self.cleaned_data

class NewChannelForm(forms.ModelForm):
    chosensensors = forms.ModelMultipleChoiceField(required=False,widget=forms.CheckboxSelectMultiple, queryset=Sensor.objects.all())
    class Meta:
        model = Channel
        fields = ('name',)

    def __init__(self, request=None, *args, **kwargs):
        super(NewChannelForm, self).__init__(*args, **kwargs)
        self.fields['name'].widget.attrs.update({'required': 'required'})
    def clean(self):
        return self.cleaned_data        



class SensorForms(forms.ModelForm):
    
    class Meta:
        model = Sensor
        fields = ('name', 'bridge', 'NETWORKID', 'NODEID', 'VERSION', 'TYPE')

    def __init__(self, request=None, *args, **kwargs):
        super(SensorForms, self).__init__(*args, **kwargs)

    def clean(self):
        return self.cleaned_data

class BridgeForms(forms.ModelForm):
    
    class Meta:
        model = Sensor
        fields = ('name', 'NETWORKID', 'NODEID', 'VERSION')

    def __init__(self, request=None, *args, **kwargs):
        super(BridgeForms, self).__init__(*args, **kwargs)

    def clean(self):
        return self.cleaned_data

