package Door::Open;
use Dancer2;
use Dancer2::Plugin::HTTP::Auth::Extensible;

our $VERSION = '0.1';

our $opened = 0;

get '/' => sub {
    template 'index';
};

get '/open' => http_require_authentication sub {
	$opened = 1;
	return 'Open noted!';
};

get '/close' => http_require_authentication sub {
	$opened = 0;
	return 'Close noted!';
};

get '/status' => sub {
	if ($opened) {
		return "It's open!";
	} else {
		return "It's closed!";
	}
};

true;
