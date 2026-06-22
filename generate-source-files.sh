echo "Generating Source Files..."

echo "Generating GLFW Wayland Source Files..."
if [ "$XDG_SESSION_TYPE" = "wayland" ]; then
	echo "Wayland Session Detected, Generating Source Files..."
	wayland-scanner client-header /usr/share/wayland/wayland.xml third-party/glfw/src/wayland-client-protocol.h
	wayland-scanner client-header /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml third-party/glfw/src/wayland-xdg-shell-client-protocol.h
	wayland-scanner client-header /usr/share/wayland-protocols/stable/viewporter/viewporter.xml third-party/glfw/src/wayland-viewporter-client-protocol.h
	wayland-scanner client-header /usr/share/wayland-protocols/unstable/xdg-decoration/xdg-decoration-unstable-v1.xml third-party/glfw/src/wayland-xdg-decoration-client-protocol.h
	wayland-scanner client-header /usr/share/wayland-protocols/unstable/relative-pointer/relative-pointer-unstable-v1.xml third-party/glfw/src/wayland-relative-pointer-unstable-v1-client-protocol.h
	wayland-scanner client-header /usr/share/wayland-protocols/unstable/pointer-constraints/pointer-constraints-unstable-v1.xml third-party/glfw/src/wayland-pointer-constraints-unstable-v1-client-protocol.h
	wayland-scanner client-header /usr/share/wayland-protocols/unstable/idle-inhibit/idle-inhibit-unstable-v1.xml third-party/glfw/src/wayland-idle-inhibit-unstable-v1-client-protocol.h
	wayland-scanner private-code /usr/share/wayland/wayland.xml third-party/glfw/src/wayland-client-protocol-code.h
	wayland-scanner private-code /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml third-party/glfw/src/wayland-xdg-shell-client-protocol-code.h
	wayland-scanner private-code /usr/share/wayland-protocols/stable/viewporter/viewporter.xml third-party/glfw/src/wayland-viewporter-client-protocol-code.h
	wayland-scanner private-code /usr/share/wayland-protocols/unstable/xdg-decoration/xdg-decoration-unstable-v1.xml third-party/glfw/src/wayland-xdg-decoration-client-protocol-code.h
	wayland-scanner private-code /usr/share/wayland-protocols/unstable/relative-pointer/relative-pointer-unstable-v1.xml third-party/glfw/src/wayland-relative-pointer-unstable-v1-client-protocol-code.h
	wayland-scanner private-code /usr/share/wayland-protocols/unstable/pointer-constraints/pointer-constraints-unstable-v1.xml third-party/glfw/src/wayland-pointer-constraints-unstable-v1-client-protocol-code.h
	wayland-scanner private-code /usr/share/wayland-protocols/unstable/idle-inhibit/idle-inhibit-unstable-v1.xml third-party/glfw/src/wayland-idle-inhibit-unstable-v1-client-protocol-code.h
elif [ "$XDG_SESSION_TYPE" = "x11" ]; then
	echo "X11 Session Detected, Aborting...\n"
fi

echo "Generated Source Files Successfully!"
