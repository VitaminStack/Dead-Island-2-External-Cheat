#include <Windows.h>
#include <iostream>

void OpenConsole()
{
    AllocConsole(); // Erstellt eine neue Konsole
    freopen_s((FILE**)stdout, "CONOUT$", "w", stdout); // Verbindet std::cout mit der Konsole
    freopen_s((FILE**)stdin, "CONIN$", "r", stdin);   // Verbindet std::cin mit der Konsole
    freopen_s((FILE**)stderr, "CONOUT$", "w", stderr); // Verbindet std::cerr mit der Konsole

    std::cout << "Konsole geöffnet!" << std::endl;
}