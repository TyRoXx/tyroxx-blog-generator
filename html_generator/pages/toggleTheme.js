function setTheme() {
    if (document.cookie.indexOf('dark') === -1) {
        document.body.classList.remove('dark')
    } else {
        document.body.classList.add('dark');
    }
}

function toggleTheme() {
    if (document.cookie.indexOf('dark') === -1) {
        document.cookie = "dark=true;";
    } else {
        document.cookie = "dark=; expires=Wed 01 Jan 1970";
    }
    setTheme();
}
