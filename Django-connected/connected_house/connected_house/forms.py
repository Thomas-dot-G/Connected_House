from django import forms
from django.conf import settings
from django.contrib.auth import authenticate
from django.contrib.auth.forms import AuthenticationForm, ReadOnlyPasswordHashField

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