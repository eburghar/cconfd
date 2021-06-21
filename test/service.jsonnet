{
    "servicerc": std.manifestJsonEx({
        json: {
            key1: 1,
            key2: 2,
            password2: secrets["password"],
        }
    }, "    "),

    "service.ini": std.manifestIni({
        sections: {
            options: {
                user: secrets["account"]["data"]["data"]["user"],
                password: secrets["account"]["data"]["data"]["password"],
                password2: secrets["password"],
                path: secrets["path"],
                # dbuser: secrets["db"]["data"]["username"],
                # dbpassword: secrets["db"]["data"]["password"]
            }
        }
    })
}