from django.shortcuts import render
from django.template import RequestContext
from django.contrib.auth import logout, login, authenticate
from django.shortcuts import redirect
from django.contrib.auth.hashers import check_password, make_password
from django.views.decorators.csrf import csrf_exempt
from django.http import HttpResponse
from django.utils.encoding import smart_str

from .forms import SignInForm, SignUpForm, EditAccountForm
from website.models import User, Sensor, Data, Version, Bridge

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

def getID(identification):
    ID = identification[1:identification.index(';')]
    remaining = identification[identification.index(';')+1:]
    NETWORDID = remaining[:remaining.index(';')]
    remaining = remaining[remaining.index(';')+1:]
    GATEWAY = remaining[:remaining.index(';')]
    remaining = remaining[remaining.index(';')+1:]
    NODEID = remaining[:remaining.index(';')]
    remaining = remaining[remaining.index(';')+1:]
    TYPE = remaining[:remaining.index(';')]
    remaining = remaining[remaining.index(';')+1:]
    VERSION = remaining[:-1]
    return [ID, NETWORDID, GATEWAY, NODEID, TYPE, VERSION]

@csrf_exempt
def post_data(request):
    if request.method == 'POST':
        value = request.POST['data']
        identification = getID("[ID;NETWORKID;GATEWAY;NODEID;TYPE;VERSION]")
        print 'identification: '
        print identification
        sensor = Sensor.objects.get(name=identification[0])
        if sensor.NETWORKID is not identification[1] or sensor.bridge is not identification[2] or sensor.NODEID is not identification[3] or sensor.TYPE is not identification[4] or sensor.VERSION is not identification[5]:
            sensor.NETWORKID = identification[1]
            #sensor.bridge = identification[2]
            sensor.NODEID = identification[3]
            sensor.TYPE = identification[4]
            sensor.VERSION = identification[5]
            sensor.save()
        data = Data(sensor=Sensor.objects.get(name=identification[0]), value=value)
        data.save()
    else:
        return redirect('/')
    return HttpResponse("OK")

@csrf_exempt
def post_Version_Bridge(request):
    if request.method == 'POST':
        #identification = request.POST['id']
        identification = getID("[ID;NETWORKID;GATEWAY;NODEID;TYPE;VERSION]")
        print 'identification: '
        print identification
        bridge = Bridge.objects.get(name=identification[0])

        if bridge.VERSION is not identification[5] or bridge.NODEID is not identification[3] or bridge.VERSION is not identification[5]:
            bridge.NETWORKID = identification[1]
            bridge.NODEID = identification[3]
            bridge.VERSION = identification[5]
            bridge.save()

        if bridge.VERSION is not Version.objects.get(TYPE="Bridge"):
            response = HttpResponse(content_type='application/force-download')
            response['Content-Disposition'] = 'attachment; filename=%s' % smart_str('flash.hex')
            response['X-Sendfile'] = smart_str('versions/bridge.hex')
            return response
    else:
        return redirect('/')
    return HttpResponse("OK")

@csrf_exempt
def post_Version_Probes(request):
    if request.method == 'POST':
        #identification = request.POST['id']
        identification = getID("[ID;NETWORKID;GATEWAY;NODEID;TYPE;VERSION]")
        print 'identification: '
        print identification
        bridge = Bridge.objects.get(name=identification[0])

        probes = Sensor.objects.filter(bridge=bridge)

        for p in probes:
            print Version.objects.get(TYPE=p.TYPE)
            if p.VERSION is not Version.objects.get(TYPE=p.TYPE):
                response = HttpResponse(content_type='application/force-download')
                response['HTTP_ACCEPT_LANGUAGE'] =  p.NODEID
                response['Content-Disposition'] = 'attachment; filename=%s' % smart_str('flash.hex')
                response['X-Sendfile'] = smart_str('versions/%s.hex' % p.TYPE)
                return response
    else:
        return redirect('/')
    return HttpResponse("OK")
    