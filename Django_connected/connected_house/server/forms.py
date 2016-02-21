from django import forms
from django.conf import settings
from django.contrib.auth import authenticate
from django.contrib.auth.forms import AuthenticationForm, ReadOnlyPasswordHashField

from website.models import User, Data, Sensor, Bridge, Channel

# File that has all the forms used in the Django application

class SignInForm(forms.ModelForm):

    # Checkbox for the remember my ID
    remember = forms.BooleanField(label='remember', widget=forms.CheckboxInput)
    
    class Meta:
        # Form linked to the User model
        model = User
        # Hide the typed caracters for the password as dots
        widgets = {
        'password': forms.PasswordInput(),
        }
        # The other fields to show (exept remember) are the email and password linked to the model
        fields = ('email', 'password')


    def __init__(self, request=None, *args, **kwargs):
        super(SignInForm, self).__init__(*args, **kwargs)
        for f in ['email', 'password']:
            # Email and password are required field
            self.fields[f].widget.attrs.update({'required': 'required'})
            self.fields[f].required = True
        # Remember is not required or the user will have to check the box in order for the form to be valid
        self.fields['remember'].required = False

    # Necessary to validate the form
    def clean(self):
        self.cleaned_data


class SignUpForm(forms.ModelForm):

    # Second password to avoid mistakes
    password2 = forms.CharField(label='Confirm password', widget=forms.PasswordInput)
    
    class Meta:
        # Form linked to the User model
        model = User
        # Hide the typed caracters for the password as dots
        widgets = {
        'password': forms.PasswordInput(),
        }
        # The other fields to show
        fields = ('name', 'email', 'timezone', 'password')


    def __init__(self, request=None, *args, **kwargs):
        super(SignUpForm, self).__init__(*args, **kwargs)
        for f in ['email', 'password']:
            self.fields[f].widget.attrs.update({'required': 'required'})
            self.fields[f].required = True

    # Necessary to validate the form
    def clean(self):
        # Only validate the form is both password are identical
        if self.data['password'] == self.data['password2']:
            return self.cleaned_data
        else:
            raise forms.ValidationError('Passwords do not match')


class EditAccountForm(forms.ModelForm):
    # Second password to avoid mistakes
    password2 = forms.CharField(label='Confirm password', widget=forms.PasswordInput)
    
    class Meta:
        # Form linked to the User model
        model = User
        # Hide the typed caracters for the password as dots
        widgets = {
        'password': forms.PasswordInput(),
        }
        # The other fields to show
        fields = ('name', 'timezone', 'password')


    def __init__(self, request=None, *args, **kwargs):
        super(EditAccountForm, self).__init__(*args, **kwargs)
        for f in ['timezone', 'name']:
            self.fields[f].widget.attrs.update({'required': 'required'})
            self.fields[f].required = True
        for f in ['password']:
            # Second password not required for that particular form (it is an update)
            self.fields[f].widget.attrs.update({'required': 'required'})
            self.fields[f].required = False

    # Necessary to validate the form
    def clean(self):
        self.cleaned_data
     


class NewChannelForm(forms.ModelForm):

    #List of all the sensors
    chosensensors = forms.ModelMultipleChoiceField(required=False,widget=forms.CheckboxSelectMultiple, queryset=Sensor.objects.all())
    description = forms.CharField( widget=forms.Textarea(attrs={'class': 'tall'}) )
    class Meta:
        # Form linked to the Channel model
        model = Channel
        # Only their name are used to select them
        fields = ('name',)

    def __init__(self, request=None, *args, **kwargs):
        super(NewChannelForm, self).__init__(*args, **kwargs)
        self.fields['name'].widget.attrs.update({'required': 'required'})
    
    # Necessary to validate the form
    def clean(self):
        return self.cleaned_data        



class SensorForms(forms.ModelForm):
    
    class Meta:
        # Form linked to the Sensor model
        model = Sensor
        # The other fields to show
        fields = ('name', 'id', 'bridge', 'NETWORKID', 'NODEID', 'VERSION', 'TYPE')

    def __init__(self, request=None, *args, **kwargs):
        super(SensorForms, self).__init__(*args, **kwargs)
    
    # Necessary to validate the form
    def clean(self):
        sensor = Sensor.objects.all().filter(name=self.data['name'])
        if sensor.count() > 0:
            raise forms.ValidationError('Name already used')
        else:
            return self.cleaned_data

class BridgeForms(forms.ModelForm):
    
    class Meta:
        # Form linked to the Sensor model
        model = Bridge
        # The other fields to show
        fields = ('name', 'id', 'NETWORKID', 'NODEID', 'VERSION')

    def __init__(self, request=None, *args, **kwargs):
        super(BridgeForms, self).__init__(*args, **kwargs)
    
    # Necessary to validate the form
    def clean(self):
        bridge = Bridge.objects.all().filter(name=self.data['name'])
        if bridge.count() > 0:
            raise forms.ValidationError('Name already used')
        else:
            return self.cleaned_data

