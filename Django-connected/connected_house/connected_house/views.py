from django.shortcuts import render
from django.template import RequestContext

from .forms import SignInForm

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

            return _standard_user_auth_login(request, user)
    else:
        form = SignInForm(request)
    context['login_form'] = form
    return render(request,'templates/signin.html', context)
