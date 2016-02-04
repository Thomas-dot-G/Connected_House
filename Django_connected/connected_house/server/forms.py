from django import forms
from django.conf import settings
from django.contrib.auth import authenticate
from django.contrib.auth.forms import AuthenticationForm, ReadOnlyPasswordHashField

from website.models import User

class SignInForm(AuthenticationForm):
    def __init__(self, request=None, *args, **kwargs):
        super(SignInForm, self).__init__(request, *args, **kwargs)
        self.fields['username'].widget = forms.EmailInput(attrs={
            'placeholder': 'Email Address',
            'autofocus': 'true',
            'class': 'form-control',
            'required': 'true'
        })
        self.fields['username'].label = ''
        self.fields['username'].required = True

        self.fields['password'].widget = forms.PasswordInput(attrs={
            'placeholder': 'Password',
            'class': 'form-control',
            'required': 'true',
            'autocomplete': 'off'
        })
        self.fields['password'].label = ''
        self.fields['password'].required = True

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
