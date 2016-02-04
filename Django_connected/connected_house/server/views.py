from django.shortcuts import render
from django.template import RequestContext
from django.contrib.auth import logout, login, authenticate
from django.shortcuts import redirect

from .forms import SignInForm, SignUpForm
from website.models import User

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
    print 'signup'
    if request.method == 'POST':
        form = SignUpForm(request, data=request.POST)
            
        is_valid = form.is_valid()
        print 'isvalid?' 
        print is_valid   
        if is_valid:
            print 'here'
            name = form.cleaned_data['name']
            email = form.cleaned_data['email']
            password = form.cleaned_data['password']
            password2 = form.cleaned_data['password2']
            timezone = form.cleaned_data['timezone']
            user = User(name=name, email=email, password=password, timezone=timezone)
            user.save()
            return redirect('/dashboard/')
    else:
        form = SignUpForm()
    context['login_form'] = form
    return render(request,'templates/signup.html', context)

def dashboard(request):
    context = {"page":"dashboard"}
    
    return render(request,'templates/dashboard.html', context)
