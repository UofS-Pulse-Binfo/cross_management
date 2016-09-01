/**
 * Add a Site Tour to the Cross Management Report Crosses Form
 *
 * Library used: Shepherd.js
 * NOTE: Other than attaching this file & the Shepherd.js library to the form
 * no other components or steps are needed.
 */

(function ($) {
  Drupal.behaviors.crossmanageReportCrossesFormShepherd = {
    attach: function (context, settings) {

    // Override default functionality for any link with the tour-start-link
    // class and instead start the full tour.
    $('a.tour-start-link').live('click', function(event) {
        event.preventDefault();
        // Check if a tour is already going to avoid duplicates
        // OR we could cancel any current tours and start one fresh using: Shepherd.activeTour.cancel();
        if (null == Shepherd.activeTour) {
          startFullTour();
        }
    });

    function startFullTour() {

      // Scroll to the top of the page to make sure my first couple of elements
      // are on the screen.
      // NOTE: we scroll manually because the Shepherd scroll functionality
      // doesn't produce the results we want (ie: arrow disappears)
//       $('html, body').animate({
//         scrollTop: $("#header").offset().top
//       }, 1000);

      // Initialize the tour.
      tour = new Shepherd.Tour({
        defaults: {
          classes: 'shepherd-theme-arrows',
          showCancelLink: true,
          //scrollTo: true,
        }
      });

      // We prefer a Back and Next button on most of our steps so we'll define
      // them first and re-use them during each step definition.

      var attachBack = null;
      var attachCurrent = '#cross_manage_crossingblock_register_crosses_form > thead';
      var attachNext = '#edit-crosses-0-cross-num';

      // STEP #1: Highlight the full table.
      //-----------------------------------------
       tour.addStep({
           title: 'Registered Crosses',
           text: 'The following table allows you to register the crosses that have already been made for this crossing block.',
           attachTo: attachCurrent + ' top',
       });

       attachBack = attachCurrent;
       attachCurrent = attachNext;
       attachNext = '#edit-crosses-0-type';

       // STEP #2: Introduce cross numbers
       //-----------------------------------------
       tour.addStep({
           text: 'When adding a new cross, a cross number will automatically be assigned.',
           attachTo: attachCurrent + ' right',
           buttons: Drupal.kptheme.shepherd.scrollingButtons(attachBack, attachNext),
       });

       attachBack = attachCurrent;
       attachCurrent = attachNext;
       attachNext = '#edit-crosses-0-selfed';

       // STEP #3: Cross type and species dropdowns
       //-----------------------------------------
       tour.addStep({
           title: 'Cross Type and Species',
           text: '<p>In the first dropdown, select the type of cross, whether it be a single, double, triple, multiple or backcross.</p><p>The second dropdown is used to indicate the species.',
           attachTo: attachCurrent + ' top',
           buttons: Drupal.kptheme.shepherd.scrollingButtons(attachBack, attachNext),
       });

       attachBack = attachCurrent;
       attachCurrent = attachNext;
       attachNext = '#edit-crosses-0-num-seeds';

       // STEP #4: Self-pollination box.
       //-----------------------------------------
       tour.addStep({
           title: 'Self-pollination',
           text: 'In such a case where self-pollination by one of the parents is suspected to have occurred, check this box.',
           attachTo: attachCurrent + ' top',
           buttons: Drupal.kptheme.shepherd.scrollingButtons(attachBack, attachNext),
       });

       attachBack = attachCurrent;
       attachCurrent = attachNext;
       attachNext = '#edit-crosses-0-maternal-parent';

       // STEP #5: Seed counts
       //-----------------------------------------
       tour.addStep({
           title: 'Seed Count',
           text: '<p>This is where you report the number of seeds produced by the cross.</p>',
           attachTo: attachCurrent + ' right',
           buttons: Drupal.kptheme.shepherd.scrollingButtons(attachBack, attachNext),
       });

       attachBack = attachCurrent;
       attachCurrent = attachNext;
       attachNext = '#edit-crosses-0-data-cotyledon-colour';

       // STEP #6: Parents
       //-----------------------------------------
       tour.addStep({
           title: 'Select Parents',
           text: '<p>Here is where the parents are entered, while ensuring that maternal is in the first box and paternal is in the one below it. Simply start typing the parent\'s name and select it from the dropdown that appears.</p>',
           attachTo: attachCurrent + ' top',
           buttons: Drupal.kptheme.shepherd.scrollingButtons(attachBack, attachNext),
        });

       attachBack = attachCurrent;
       attachCurrent = attachNext;
       attachNext = '#edit-crosses-0-action';

       // STEP #7: Characteristics
       //-----------------------------------------
       tour.addStep({
           title: 'Characteristics',
           text: '<p>If you can clearly identify the cotyledon colour or seed coat colour of the seeds produced by the cross, you can enter them here.</p>',
           attachTo: attachCurrent + ' top',
           buttons: Drupal.kptheme.shepherd.scrollingButtons(attachBack, attachNext),
        });

       attachBack = attachCurrent;
       attachCurrent = attachNext;
       attachNext = '#edit-submit-cb';

       // STEP #8: Add Cross
       //-----------------------------------------
       tour.addStep({
           id: 'add-cross-step',
           title: 'Add!',
           text: 'When you\'ve finished filling out details for this cross, click here to add it to the form. A new row will then appear to allow you to enter an additional cross. You can still edit a cross after you\'ve added additional rows.',
           attachTo: attachCurrent + ' left',
           buttons: Drupal.kptheme.shepherd.scrollingButtons(attachBack, attachNext),
       });

       attachBack = attachCurrent;
       attachCurrent = attachNext;
       // This ensures that when 'Done!' is pressed, the user is brought back to the location
       // of the Help button.
       attachNext = '#cross-manage-crossingblock-assign-cross-num-form > div > a.tour-start-link.tour-image-link > img';

       // STEP #8: Finalize Crossing Block
       //-----------------------------------------
       tour.addStep({
           title: 'Finalize Crossing Block',
           text: 'Don\'t forget to finalize the crossing block after you\'ve entered all of the crosses!',
           attachTo: attachCurrent + ' right',
           buttons: Drupal.kptheme.shepherd.scrollingButtons(attachBack, attachNext, true),
      });

      // Now actually start the tour
      tour.start();
    }

  }};
}(jQuery));
