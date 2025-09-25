let projetorLigado = false;

let telaCongelada = false;

let volume = 50; // valor inicial
const maxVolume = 100;
const minVolume = 0;

let zoom = 50;
const maxZoom = 100;
const minZoom = 0;

// Botão POWER
document.getElementById("power").addEventListener("click", function() {
    projetorLigado = !projetorLigado; // alterna ligado/desligado

    if (!projetorLigado) {
        telaCongelada = false;
    }

    if (projetorLigado) {
        document.getElementById("status").innerHTML =
            '<div class="alert alert-success" role="alert">Projetor ligado!</div>';
    } else {
        document.getElementById("status").innerHTML =
            '<div class="alert alert-danger" role="alert">Projetor desligado!</div>';
    }
});

// Botão FREEZE
document.getElementById("freeze").addEventListener("click", function() {
    telaCongelada = !telaCongelada
    if(projetorLigado){
        if (telaCongelada) {
            document.getElementById("status").innerHTML =
                '<div class="alert alert-success" role="alert">Tela congelada!</div>';
        } else{
            document.getElementById("status").innerHTML =
                '<div class="alert alert-danger" role="alert">Tela descongelada!</div>';
        }
        
    } else {
        document.getElementById("status").innerHTML =
            '<div class="alert alert-warning" role="alert">Ligue o projetor antes!</div>';
    }
});

// Botão VOLUME +
document.getElementById("volume-up").addEventListener("click", function() {
    if(projetorLigado){
        if(volume < maxVolume) {
        volume += 10;
        if(volume > maxVolume) volume = maxVolume;
        document.getElementById("status").innerHTML =
            '<div class="alert alert-info" role="alert">Volume: ' + volume + '</div>';
        } else {
        document.getElementById("status").innerHTML =
            '<div class="alert alert-info" role="alert">Volume: 100</div>';
        }
        
    } else {
        document.getElementById("status").innerHTML =
            '<div class="alert alert-warning" role="alert">Ligue o projetor antes!</div>';
    }
    
});

// Botão VOLUME -
document.getElementById("volume-down").addEventListener("click", function() {
    if(projetorLigado){
        if(volume > minVolume) {
        volume -= 10;
        if(volume < minVolume) volume = minVolume;
        document.getElementById("status").innerHTML =
            '<div class="alert alert-info" role="alert">Volume: ' + volume + '</div>';
        } else {
        document.getElementById("status").innerHTML =
            '<div class="alert alert-info" role="alert">Volume: 0</div>';
        }
        
    } else {
        document.getElementById("status").innerHTML =
            '<div class="alert alert-warning" role="alert">Ligue o projetor antes!</div>';
    }
});

document.getElementById("zoom-in").addEventListener("click", function() {
    if(projetorLigado){
        if (zoom < maxZoom) {
        zoom += 10;
        if (zoom > maxZoom) zoom = maxVolume;
        document.getElementById("status").innerHTML =
            '<div class="alert alert-info" role="alert">Zoom: ' + zoom + '%</div>'
        } else {
        document.getElementById("status").innerHTML =
            '<div class="alert alert-info" role="alert">Zoom: 100%</div>';
        }
        
    } else {
        document.getElementById("status").innerHTML =
            '<div class="alert alert-warning" role="alert">Ligue o projetor antes!</div>';
    }
    
})
document.getElementById("zoom-out").addEventListener("click", function() {
    if(projetorLigado){
        if (zoom > minZoom) {
        zoom -= 10;
        if (zoom < minZoom) zoom = minVolume;
        document.getElementById("status").innerHTML =
            '<div class="alert alert-info" role="alert">Zoom: ' + zoom + '%</div>'
        } else {
        document.getElementById("status").innerHTML =
            '<div class="alert alert-info" role="alert">Zoom: 0%</div>';
        }
        
    } else {
        document.getElementById("status").innerHTML =
            '<div class="alert alert-warning" role="alert">Ligue o projetor antes!</div>';
    }
    
})
