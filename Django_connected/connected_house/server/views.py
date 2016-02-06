from django.shortcuts import render
from django.template import RequestContext
from django.contrib.auth import logout, login, authenticate
from django.shortcuts import redirect
from  django.contrib.auth.hashers import check_password, make_password

from .forms import SignInForm, SignUpForm, EditAccountForm
from website.models import User

# Create your views here.

def index(request):
    context = {"page":"index"}
    user_email = request.session.get('user', None)
    if user_email:
        try:
            user = User.objects.get(email=user_email)
        except Exception as e:
            request.session.flush()
            return redirect('/')
        if user:
            context.update({"user": user_email})

    return render(request,'templates/index.html', context)

def login(request, user):
    request.session['user'] = user.email
    return redirect('/dashboard/')

def logout(request):
    request.session.flush()
    return redirect('/')

def check_auth(request):
    user_email = request.session.get('user', None)
    context = {"user": user_email}
    if not user_email:
        return redirect('/signin')
    else:
        return user_email 

def signin(request):
    context = {"page":"signin", "loginremember": request.session.get('loginremember')}
    if request.method == 'POST':
        form = SignInForm(request, data=request.POST)
            
        is_valid = form.is_valid()
        print is_valid
        if is_valid:
            email = form.cleaned_data['email']
            password = form.cleaned_data['password']
            remember = form.cleaned_data['remember']
            # Use Cookie
            try:
                user = User.objects.get(email=email)
            except Exception as e:
                print 'HGGGGGGGGGGGG'
                context['login_form'] = form
                context.update({"error": True})
                return render(request,'templates/signin.html', context)
            if check_password(password, user.password):
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
    context.update({"user": check_auth(request)})
    return render(request,'templates/dashboard.html', context)


def myaccount(request):
    context = {"page":"myaccount"}
    email = check_auth(request)
    context = {"user": email}
    user = User.objects.get(email=email)
    if user:
        context.update({
        'email': email,
        'name': user.name,
        'timezone': user.timezone#,
        #'api': user.api
        })
        # We do not put user to avoid placing the password in the template
        # even if it is hashed
    else:
        return redirect('/signin')
    return render(request,'templates/myaccount.html', context)

def generateapi(request):
    # This page (the html one) should ask for the user password to be type if a new key is requested
    # TODO generate new api key
    return redirect('/myaccount')

def myaccount_delete(request):
    # This page (the html one) should ask for the user password
    email = check_auth(request)
    user = User.objects.get(email=email)
    user.delete()
    return logout(request)

def myaccount_edit(request):
    context = {"page":"account_edit"}
    email = check_auth(request)
    user = User.objects.get(email=email)
    context.update({"user": email})
    data = {'name': user.name, 'timezone': user.timezone}
    if request.method == 'POST':
        form = EditAccountForm(request, data=request.POST)
            
        is_valid = form.is_valid()

        if is_valid:
            name = form.cleaned_data['name']
            password = form.cleaned_data['password']
            password2 = form.cleaned_data['password2']
            timezone = form.cleaned_data['timezone']
            if check_password(password2, user.password):
                user.name = name
                user.timezone = timezone
                if password is not ('' or None):
                    user.password = make_password(password, salt=name+'connect2', hasher='default')
                user.save()
                request.session['user'] = user.email
                return redirect('/myaccount/')
            else:
                context.update({"error": "Your password was not recognised"})
                context['account_form'] = form
                return render(request,'templates/account_edit.html', context)
    else:
        form = EditAccountForm(initial=data)
    context['account_form'] = form
    return render(request,'templates/account_edit.html', context)