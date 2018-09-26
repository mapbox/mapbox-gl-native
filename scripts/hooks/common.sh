if [ "$1" = '--install' ]; then
    mkdir -p ".git/hooks"
    if [ -e ".git/hooks/$HOOK" ] || [ -L ".git/hooks/$HOOK" ]; then
        if [ "$(readlink .git/hooks/$HOOK)" != "scripts/hooks/$HOOK" ]; then
            echo "Can't install $HOOK because it has custom content"
        else
            echo "$HOOK hook is already installed"
        fi
        exit 1
    fi
    echo "Installing $HOOK hook"
    ln -s ../../scripts/hooks/$HOOK .git/hooks/$HOOK
    exit 0
elif [ "$1" = '--uninstall' ]; then
    if [ "$(readlink .git/hooks/$HOOK)" = "scripts/hooks/$HOOK" ]; then
        echo "Removing $HOOK hook"
        rm .git/hooks/$HOOK
        exit 0
    else
        echo "$HOOK hook is not installed"
        exit 1
    fi
fi
