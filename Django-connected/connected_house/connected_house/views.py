from django.shortcuts import render
from django.template import RequestContext

# Create your views here.

def index(request):
    context = {"titre":"titre2"}
    return render(request,'index.html', context)
