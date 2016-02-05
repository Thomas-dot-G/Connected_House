from django.shortcuts import render
from django.template import RequestContext
from django.contrib.auth import logout, login, authenticate
from django.shortcuts import redirect
from  django.contrib.auth.hashers import check_password, make_password

from .forms import SignInForm, SignUpForm
from website.models import User

# Create your views here.

def index(request):
    context = {"page":"index"}
    user_email = request.session.get('user')
    if user_email:
        user = User.objects.get(email=user_email)
        if user:
            return redirect('/dashboard/')
    return render(request,'templates/index.html', context)

def login(request, user):
    request.session['user'] = user.email
    return redirect('/dashboard/')

def logout(request):
    request.session.flush()
    return redirect('/')

def signin(request):
    context = {"page":"signin"}
    if request.method == 'POST':
        form = SignInForm(request, data=request.POST)
            
        is_valid = form.is_valid()
            
        if is_valid:
            email = form.cleaned_data['email']
            password = form.cleaned_data['password']
            user = User.objects.get(email=email)
            if check_password(password, user.password):
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

        if is_valid:
            name = form.cleaned_data['name']
            email = form.cleaned_data['email']
            password = form.cleaned_data['password']
            password2 = form.cleaned_data['password2']
            timezone = form.cleaned_data['timezone']
            user = User(name=name, email=email, password=make_password(password, salt=name+'connect2', hasher='default'), timezone=timezone)
            user.save()
            return login(request, user)
    else:
        form = SignUpForm()
    context['login_form'] = form
    return render(request,'templates/signup.html', context)

def dashboard(request):
    context = {"page":"dashboard"}
    
    return render(request,'templates/dashboard.html', context)
