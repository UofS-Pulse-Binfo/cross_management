/**
 * Add a Site Tour to the Cross Management F1's Form
 *
 * Library used: Shepherd.js
 * NOTE: Other than attaching this file & the Shepherd.js library to the form
 * no other components or steps are needed.
 */

(function ($) {
  Drupal.behaviors.crossmanageF1sFormShepherd = {
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
          title: 'F1\'s Table',
          text: 'The following table lists all of the crosses made in the previous crossing block and allows you to tell the field lab how they should be advanced.',
          attachTo: 'table#cross_manage_crossingblock_f1_form top'
      });

      // STEP #2: Define each row as an F1.
      //-----------------------------------------
      tour.addStep({
          text: 'Each row corresponds to an F1 from the previous crossing block and is identified by the cross number shown in this column.',
          attachTo: '#f1-row0-cross_num right',
          buttons: myButtons
      });

      // STEP #3: Seed Produced column.
      //-----------------------------------------
      tour.addStep({
          title: 'F1 Seed Produced',
          text: '<p>The original number of seeds collected from the cross will be listed in this column, if it has been entered into KnowPulse.</p> <p>If you find out this number as part of your cross design, please enter it in the textbox provide but remember, this number should not be updated and should not become smaller as seeds are used.</p>',
          attachTo: '#f1-row0-seed_num right',
          buttons: myButtons
      });

      // STEP #4: Seed Allocations column.
      //-----------------------------------------
      tour.addStep({
          title: 'Seed Allocation',
          text: '<p>This column allows you to indicate how much seed should</p> <ol> <li>Go into this crossing block</li> <li>Be advanced to the next generation by growing it out in the nursery</li> <li>Be reserved for a "Rainy Day"</li> </ol>',
          attachTo: '#f1-row0-seed_allocate right',
          buttons: myButtons
      });

      // STEP #5: Breeder Notes & Instructions to the field lab column.
      //-----------------------------------------
      tour.addStep({
          title: 'Notes & Instructions',
          text: '<p>You can enter random notes to yourself in the "Breeder\'s Notes" and any specific instructions or notes to the field lab should be entered in the "Instructions" section.</p>',
          attachTo: '#f1-row0-notes left',
          buttons: myButtons
      });

      // STEP #6: Highlight the crosses to be made.
      //-----------------------------------------
      tour.addStep({
          title: 'Crosses to be Made',
          text: '<p>This is where you indicate what germplasm you would like to cross with the current F1. Each dropdown represents a unique cross combination and can be any of the parents you added in the previous form or any of the F1s mentioned in this form.</p> <p>NOTE: The current F1 will be the maternal parent and the germplasm chosen to be crossed with it will be the paternal parent.</p>',
          attachTo: '#f1-row0-cross_to left',
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

      // STEP #7: Save Button.
      //-----------------------------------------
      tour.addStep({
          id: 'save-button-step',
          title: 'Save!',
          text: 'Be sure to save your progress whenever you\'ve made changes to the form, but if it isn\'t ready to be sent to the field lab.',
          attachTo: '#edit-save-f1 top',
          buttons: myButtons
      });

      // STEP #8: Submit to Field Lab button.
      //-----------------------------------------
      tour.addStep({
          title: 'Send to Field Lab!',
          text: 'When the form has been completed to your satisfaction, click "Send to Field Lab".',
          attachTo: '#edit-submit-f1 top',
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
