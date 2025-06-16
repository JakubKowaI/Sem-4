use HTTP::Daemon;
use HTTP::Status;

my $d = HTTP::Daemon->new(
    LocalAddr => '127.0.0.1',
    LocalPort => 4321,
) || die;

print "Please contact me at: <URL:", $d->url, ">\n";

while (my $c = $d->accept) {
    while (my $r = $c->get_request) {
        if ($r->method eq 'GET') {
            my $header_dump = "";
            for my $h ($r->headers->header_field_names) {
                $header_dump .= "$h: " . $r->header($h) . "\n";
            }
            $c->send_response(
                HTTP::Response->new(200, 'OK', ['Content-Type' => 'text/plain'], $header_dump)
            );
        } else {
            $c->send_error(RC_FORBIDDEN);
        }
    }
    $c->close;
    undef($c);
}
