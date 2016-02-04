function slide() {
	// Construct target
	if (jQuery(this).data('target-relative')) {
		target = eval('jQuery(this)' + jQuery(this).data('target-relative'));
	} else {
		target = jQuery(this).data('target');
	}
	// Toggle button class
	jQuery(this).toggleClass('collapsed');
	if (jQuery(this).data('toggle')) {
		toggle = jQuery(this).data('toggle');
		jQuery(this).toggleClass(toggle);
		//jQuery(target).toggleClass(toggle);
	}
	// Toggle visibility
	if (jQuery(target).is(':visible')) {
		jQuery(target).slideUp('fast');
	} else {
		jQuery(target).slideDown('fast');
	}
}
jQuery(document).ready(function() {
	/* Add click event to mobile button */
	jQuery('#mobile-button').click(slide);
	jQuery('#zo2-drilldown-menu .toggle-icon').click(slide);

	/* Check if browser supports SVG and if not replace files with PNG */
	if (!document.createElement('svg').getAttributeNS) {
		jQuery('img[src$=\".svg\"]').each(function() {
			jQuery(this).attr('src', jQuery(this).attr('src').replace('.svg', '.png'));
		});
	}
});
// TODO: use one logic and structure for every menu
jQuery(document).ready(function() {
	if (jQuery('.menu-docs').length) {

		// Some menu specific styling
		h = jQuery('#zt-content').height();
		jQuery('.menu-docs').css('min-height', h);
		jQuery('#zt-content').parent().css('background-color', 'white');

		// Add icons
		jQuery('.menu-docs .parent > span').after('<icon class=\"toggle-icon\"></icon><div class=\"clearfix\"></div>');


		// Add open class to current category
		current = jQuery('.menu-docs .current');
		if (current.length) {
			jQuery(current).closest('li.parent').addClass('current-parent');
			jQuery(current).closest('li.parent').addClass('open');
		} else {
			jQuery('.menu-docs .first').addClass('open');
		}
		jQuery('.open .toggle-icon').addClass('collapsed');

		// Add click event
		jQuery('.menu-docs .parent').click(function() {
			if (jQuery(this).hasClass('open')) {
				// Do nothing
			} else {
				jQuery('.menu-docs .open').children('ul').slideUp();
				jQuery('.menu-docs .open').removeClass('open');
				jQuery('.toggle-icon.collapsed').removeClass('collapsed');
				jQuery(this).children('ul').slideDown();
				jQuery(this).addClass('open');
				jQuery('.open .toggle-icon').addClass('collapsed');
			}
		});

		// TODO: structure this more properly
		jQuery('.menu-docs').parent().after('<div class=\"hidden-desktop\" style=\"text-align: center;\"><a class=\"button view-menu\" data-target-relative=\".parent().siblings(\'.modulecontent\')\" data-toggle=\"hide\"></a></div>');
		jQuery('.view-menu').click(slide);
	}
});