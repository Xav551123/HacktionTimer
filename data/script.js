

function onButton() {
    var xhttp = new XMLHttpRequest();
    xhttp.open("GET", "on", true);
    xhttp.send();
}

function offButton() {
    var xhttp = new XMLHttpRequest();
    xhttp.open("GET", "off", true);
    xhttp.send();
}

function RestartButton() {
    var xhttp = new XMLHttpRequest();
    xhttp.open("GET", "reset", true);
    xhttp.send();
}

setInterval(function getData()
{
    var xhttp = new XMLHttpRequest();

    xhttp.onreadystatechange = function()
    {
        if(this.readyState == 4 && this.status == 200)
        {
            document.getElementById("ValeurMinute").innerHTML = this.responseText;
        }
    };

    xhttp.open("GET", "lireMinute", true);
    xhttp.send();
}, 2000);

setInterval(function getData()
{
    var xhttp = new XMLHttpRequest();

    xhttp.onreadystatechange = function()
    {
        if(this.readyState == 4 && this.status == 200)
        {
            document.getElementById("ValeurHeure").innerHTML = this.responseText;
        }
    };

    xhttp.open("GET", "lireHeure", true);
    xhttp.send();
}, 2000);

window.addEventListener('load',function getData()
{
    var xhttp = new XMLHttpRequest();
    console.log("color update");
    xhttp.onreadystatechange = function()
    {
        if(this.readyState == 4 && this.status == 200)
        {
            document.getElementById("gauche").value = this.responseText;
        }
    };

    xhttp.open("GET", "Couleurgauche", true);
    xhttp.send();
});

window.addEventListener('load',function getData()
{
    var xhttp = new XMLHttpRequest();
    console.log("color update");
    xhttp.onreadystatechange = function()
    {
        if(this.readyState == 4 && this.status == 200)
        {
            document.getElementById("centre").value = this.responseText;
        }
    };

    xhttp.open("GET", "Couleurcentre", true);
    xhttp.send();
});

window.addEventListener('load',function getData()
{
    var xhttp = new XMLHttpRequest();
    console.log("color droite");
    xhttp.onreadystatechange = function()
    {
        if(this.readyState == 4 && this.status == 200)
        {
            document.getElementById("droite").value = this.responseText;
        }
    };

    xhttp.open("GET", "Couleurdroite", true);
    xhttp.send();
});

window.addEventListener('load',function getData()
{
    var xhttp = new XMLHttpRequest();
    console.log("M1");
    xhttp.onreadystatechange = function()
    {
        if(this.readyState == 4 && this.status == 200)
        {
            document.getElementById("M1").innerHTML= this.responseText;
            
        
        }
    };

    xhttp.open("GET", "PlagesetM1", true);
    xhttp.send();
});


