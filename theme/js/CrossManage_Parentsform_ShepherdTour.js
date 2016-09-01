/**
 * Add a Site Tour to the Cross Management Parents Form
 *
 * Library used: Shepherd.js
 * NOTE: Other than attaching this file & the Shepherd.js library to the form
 * no other components or steps are needed.
 */

(function ($) {
  Drupal.behaviors.crossmanageParentsFormShepherd = {
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
      $('html, body').animate({
        scrollTop: $("#header").offset().top
      }, 1000);

      // Initialize the tour.
      tour = new Shepherd.Tour({
        defaults: {
          classes: 'shepherd-theme-arrows',
          showCancelLink: true,
        }
      });

      // We prefer a Back and Next button on most of our steps so we'll define
      // them first and re-use them during each step definition.
      myButtons = [
        {
          text: 'Back',
          action: tour.back,
          classes: 'shepherd-button-secondary'
        },
        {
          text: 'Next',
          action: tour.next,
          classes: 'shepherd-button-primary'
        }
      ];

      // STEP #1: Highlight the full table.
      //-----------------------------------------
      tour.addStep({
          title: 'Parents Table',
          text: 'The following table allows you to specify which parents will be used for this crossing block, as well as a means to tell the field lab how the crosses should be advanced.',
          attachTo: 'table#cross_manage_crossingblock_parents_form top'
      });

      // STEP #2: Define P Numbers.
      //-----------------------------------------
      tour.addStep({
          text: 'Each parent requires a parent number (or P-number): this box is automatically filled each time a new parent is added (incremented by 1), but can also be changed manually.',
          attachTo: '.first.cross-parents-row .pnum.parent-cell right',
          buttons: myButtons
      });

      // STEP #3: Declare a line as a parent.
      //-----------------------------------------
      tour.addStep({
          text: '<p>Designate which line will be a parent. This line must already be entered into KnowPulse.</p>',
          attachTo: '.first.cross-parents-row .line.parent-cell right',
          buttons: myButtons
      });

      // STEP #4: Source?
      //-----------------------------------------
      tour.addStep({
          title: 'Seed Source',
          text: '<p>Here is where you indicate seed source for planting of this parent. For example, you may specify to use Breeder\'s seed or seed from a particular plot.</p>',
          attachTo: '.first.cross-parents-row .source.parent-cell right',
          buttons: myButtons
      });

      // STEP #5: Breeder Notes & Instructions to the field lab column.
      //-----------------------------------------
      tour.addStep({
          title: 'Notes & Instructions',
          text: '<p>You can enter random notes to yourself in the "Breeder\'s Notes" and any specific instructions or notes to the field lab should be entered in the "Instructions" section.</p>',
          attachTo: '.first.cross-parents-row .notes.parent-cell top',
          buttons: myButtons
      });

      // STEP #6: Highlight the crosses to be made.
      //-----------------------------------------
      tour.addStep({
          title: 'Crosses to be Made',
          text: '<p>This is where you indicate what germplasm you would like to cross with the current parent. Each dropdown represents a unique cross combination and can be any of the parents you previously added in this form.</p> <p>NOTE: The current parent will be the maternal parent and the germplasm chosen to be crossed with it will be the paternal parent.</p>',
          attachTo: '.first.cross-parents-row .cross_to.parent-cell left',
          buttons: [
            {
              text: 'Back',
              action: tour.back,
              classes: 'shepherd-button-secondary'
            },
            {
              text: 'Next',
              action: function() {
                // Move on to the next step.
                tour.next();
                // Scroll to the bottom of the page.
                $('html, body').animate({
                  scrollTop: $("#footer-wrapper").offset().top
                }, 1000);
              },
              classes: 'shepherd-button-primary'
            }
          ]
      });

      // STEP #7: Add Parent Button.
      //-----------------------------------------
      tour.addStep({
          id: 'save-button-step',
          title: 'Add a new parent',
          text: 'When you wish to add another line as a parent, click this button and a new row will appear in the form. This also enables you to save any progress but nothing will be sent to the field lab at this point.',
          attachTo: '#edit-parents-add-row left',
          buttons: myButtons
      });

      // STEP #8: Submit Crossing Plans button.
      //-----------------------------------------
      tour.addStep({
          title: 'Send to Field Lab!',
          text: 'Once you have entered all of the lines you want as parents and the crosses you wish to make with them, click "Submit Crossing Plans" to send this information to the field lab.',
          attachTo: '#edit-submit-cb right',
          buttons: [
            {
              text: 'Back',
              action: tour.back,
              classes: 'shepherd-button-secondary'
            },
            {
              text: 'Done!',
              classes: 'shepherd-button-complete',
              action: function() { return tour.hide(); }
            }
          ]
      });

      // Now actually start the tour
      tour.start();
    }

  }};
}(jQuery));
