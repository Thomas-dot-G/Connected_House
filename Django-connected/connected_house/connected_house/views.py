from django.shortcuts import render
from django.template import RequestContext
from django.contrib.auth import logout, login, authenticate

from .forms import SignInForm, SignUpForm

# Create your views here.

def index(request):
    context = {"page":"index"}
    return render(request,'templates/index.html', context)

def signin(request):
    context = {"page":"signin"}
    if request.method == 'POST':
        form = SignInForm(request, data=request.POST)
            
        is_valid = form.is_valid()
            
        if is_valid:
            user = form.get_user()
                #TODO
            return login(request, user)
    else:
        form = SignInForm(request)
    context['login_form'] = form
    return render(request,'templates/signin.html', context)

def signup(request):
    context = {"page":"signup"}
    if request.method == 'POST':
        form = SignUpForm(request, data=request.POST)
            
        is_valid = form.is_valid()
            
        if is_valid:
            user = form.get_user()
                #TODO
            return login(request, user)
    else:
        form = SignUpForm(request)
    context['login_form'] = form
    return render(request,'templates/signup.html', context)

def dashboard(request):
    context = {"page":"dashboard"}
    
    return render(request,'templates/dashboard.html', context)
