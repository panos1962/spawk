Globals = {};

$(document).ready(function() {
	Globals.bodyDOM = $(document.body);
	Globals.mainDOM = $('#main');
	Globals.tocSetup();

	if (location.hash)
	setTimeout(function() {
		self.location = window.location;
	}, 0);
});

Globals.tocSetup = function() {
	var empty = true;

	Globals.tocDOM = $('<div>').attr('id', 'toc').
	append($('<div>').attr('id', 'tocHeader').
	append($('<a>').attr('href', '#').text('Table of contents')).
	on('click', function(e) {
		e.preventDefault();

		if ($(this).hasClass('tocHeaderClosed')) {
			$(this).removeClass('tocHeaderClosed');
			Globals.tocDOM.children('.tocLink').css('display', '');
		}

		else {
			$(this).addClass('tocHeaderClosed');
			Globals.tocDOM.children('.tocLink').css('display', 'none');
		}
	}));

	$('.tocItem').each(function() {
		var level = $(this).attr('toclevel');
		var link = $(this).attr('toclink');
		var dom;

		$(this).data('tocIndex', $('<a>').
		attr('href', '#' + link).
		addClass('tocLink').
		css('left', (level * 20) + 'px').
		html($(this).html()).
		appendTo(Globals.tocDOM));

		empty = false;
	});

	if (empty)
	return Globals;

	Globals.tocDOM.insertBefore(Globals.mainDOM);
	Globals.bodyDOM.on('click', '.tocItem', function(e) {
		$('.tocLast').removeClass('tocLast');
		$(this).data('tocIndex').addClass('tocLast');
		$(window).scrollTop(0);
	});
};
