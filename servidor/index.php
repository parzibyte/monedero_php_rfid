<?php
/*

  ____          _____               _ _           _       
 |  _ \        |  __ \             (_) |         | |      
 | |_) |_   _  | |__) |_ _ _ __ _____| |__  _   _| |_ ___ 
 |  _ <| | | | |  ___/ _` | '__|_  / | '_ \| | | | __/ _ \
 | |_) | |_| | | |  | (_| | |   / /| | |_) | |_| | ||  __/
 |____/ \__, | |_|   \__,_|_|  /___|_|_.__/ \__, |\__\___|
         __/ |                               __/ |        
        |___/                               |___/         
    
____________________________________
/ Si necesitas ayuda, contáctame en \
\ https://parzibyte.me               /
 ------------------------------------
        \   ^__^
         \  (oo)\_______
            (__)\       )\/\
                ||----w |
                ||     ||
Creado por Parzibyte (https://parzibyte.me).
------------------------------------------------------------------------------------------------
            | IMPORTANTE |
Si vas a borrar este encabezado, considera:
Seguirme: https://parzibyte.me/blog/sigueme/
Y compartir mi blog con tus amigos
También tengo canal de YouTube: https://www.youtube.com/channel/UCroP4BTWjfM0CkGB6AFUoBg?sub_confirmation=1
Twitter: https://twitter.com/parzibyte
Facebook: https://facebook.com/parzibyte.fanpage
Instagram: https://instagram.com/parzibyte
Hacer una donación vía PayPal: https://paypal.me/LuisCabreraBenito
------------------------------------------------------------------------------------------------
*/ ?>
<?php
// Este archivo lo va a estar consultando el lector, recordemos que nos dará su saldo
include_once "constantes.php";
// Crear archivos en caso de que no existan
if (!file_exists(ARCHIVO_ORDEN_LECTOR)) {
    touch(ARCHIVO_ORDEN_LECTOR);
}

if (!file_exists(ARCHIVO_RESPUESTA_LECTOR)) {
    touch(ARCHIVO_RESPUESTA_LECTOR);
}
$saldo = floatval($_GET["saldo"]);
// Si no le han dado una orden, simplemente imprimimos "" y el  lector no hará nada con la tarjeta
$contenido = trim(file_get_contents(ARCHIVO_ORDEN_LECTOR));
if ($contenido === "") {
    echo "";
} else {
    // En caso de que haya una orden...
    $opciones = explode(";", $contenido);
    // Extraemos la orden (descontar o recargar) y el saldo
    $accion = $opciones[0];
    $monto = floatval($opciones[1]);
    if ($accion === "d") {
        // Si la acción es descontar pero el saldo no es suficiente, lo indicamos en la página web (el lector hará la misma comprobación)
        if ($saldo - $monto < 0) {
            file_put_contents(ARCHIVO_RESPUESTA_LECTOR, "Saldo insuficiente. Retire la tarjeta");
        } else {
            file_put_contents(ARCHIVO_RESPUESTA_LECTOR, "Compra exitosa. Retire la tarjeta");
        }
    } else if ($accion == "r") {
        file_put_contents(ARCHIVO_RESPUESTA_LECTOR, "Recarga exitosa. Retire la tarjeta");
    }
    echo $contenido;
    // Y limpiamos la orden del lector
    file_put_contents(ARCHIVO_ORDEN_LECTOR, "");
}
