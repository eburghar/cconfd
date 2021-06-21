{
    "odoorc": std.manifestIni({
        sections: {
            options: {
                admin_passwd: "admin",
                addons_path: "/usr/share/odoo/enterprise-addons,/usr/share/odoo/addons,/var/www/odoo12/src/addons",
                csv_internal_sep: ",",
                data_dir: "/var/odoo/odoo12/data",
                db_host: "/var/postgresql/run",
                db_maxconn: "64",
                db_name: false,
                db_password: secrets["db"]["data"]["password"],
                db_port: false,
                db_template: "template1",
                db_user: secrets["db"]["data"]["username"],
                dbfilter: "local12_.*",
                debug_mode: false,
                demo: false,
                dev_mode: false,
                email_from: false,
                geoip_database: "/usr/share/GeoIP/GeoLiteCity.dat",
                import_partial: "",
                limit_memory_hard: 2684354560,
                limit_memory_soft: 2147483648,
                limit_request: 8192,
                limit_time_cpu: 60,
                limit_time_real: 120,
                list_db: true,
                log_db: false,
                log_db_level: "warning",
                log_handler: "[':DEBUG']",
                log_level: "debug",
                logfile: "/var/www/odoo12/logs/odoo.log",
                logrotate: false,
                longpolling_port: 8072,
                max_cron_threads: 2,
                osv_memory_age_limit: 1.0,
                osv_memory_count_limit: false,
                pg_path: "None",
                pidfile: false,
                proxy_mode: true,
                reportgz: false,
                server_wide_modules: "None",
                smtp_password: false,
                smtp_port: "25",
                smtp_server: "localhost",
                smtp_ssl: false,
                smtp_user: false,
                syslog: false,
                test_commit: false,
                test_enable: false,
                test_file: false,
                test_report_directory: false,
                translate_modules: "['all']",
                unaccent: false,
                without_demo: false,
                workers: 4,
                xmlrpc: true,
                xmlrpc_interface: "",
                xmlrpc_port: 8069
            }
        }}),

    "uwsgi.xml": std.manifestXmlJsonml([
        "uwsgi",
            ["plugin", "python"],
            ["master"],
            ["socket", "/var/uwsgi/odoo.sock"],
            ["stats", "/var/uwsgi/odoo-stats.sock"],
            ["chmod-socket", "666"],
            ["thread", "2"],
            ["processes", "4"],
            ["virtualenv", "var/www/odoo12/python"],
            ["mount", "/=/var/www/odoo10/bin/odoo_wsgi.py"]
        ])
}