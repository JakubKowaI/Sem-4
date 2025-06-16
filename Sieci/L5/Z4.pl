use HTTP::Daemon;
use HTTP::Status;
use File::Spec;

my $root = "./site";  # katalog z plikami HTML
my $d = HTTP::Daemon->new(
    LocalAddr => '127.0.0.1',
    LocalPort => 4321,
) || die;

print "Serwer działa pod: <URL:", $d->url, ">\n";

while (my $c = $d->accept) {
    while (my $r = $c->get_request) {
        if ($r->method eq 'GET') {
            my $uri_path = $r->uri->path;

            # domyślnie index.html
            $uri_path =~ s|/$|/index.html|;
            $uri_path =~ s|^/||;  # usuń leading slash

            my $full_path = File::Spec->catfile($root, $uri_path);

            if (-f $full_path) {
                $c->send_file_response($full_path);
            } else {
                $c->send_error(RC_NOT_FOUND);
            }
        } else {
            $c->send_error(RC_FORBIDDEN);
        }
    }
    $c->close;
    undef($c);
}
