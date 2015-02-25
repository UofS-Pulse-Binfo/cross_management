<?php

  // Set the title if we are at a particular crossing block.
  if (!$is_launchpad) {
    $title = format_string('!species: !season !year Crossing Block',
      array('!species' => str_replace('-',' ',$crossingblock_species),
        '!season' => $crossingblock_season,
        '!year' => $crossingblock_year));
    drupal_set_title($title);
  }

  // Checkmark image.
  $checkmark_img_html = '<img src="http://images.sodahead.com/polls/004121505/3412401521_transparent_green_checkmark_md_answer_1_xlarge.png" width=20 height=20 style="border:none;">';
?>

<script>
  $(function() {
    $( "#accordion" ).accordion({
      collapsible: true,
      active: <?php print $form_steps['active_panel']; ?>,
      heightStyle: "content"
    });
  });
</script>

<script>
  $(function() {
    $( "#step-pages.step-1" ).tabs({
      event: "mouseover"
    });
    $( "#step-pages.step-2" ).tabs({
      event: "mouseover"
    });
    $( "#step-pages.step-3" ).tabs({
      event: "mouseover"
    });
    $( "#step-pages.step-4" ).tabs({
      event: "mouseover"
    });
  });
</script>

<div id="accordion" class="workflow-diagram">
  <h3 class="step breeder">
  <?php if ($form_steps['step1']['complete']) { print $checkmark_img_html; } ?>
  Plan Crosses by selecting Parents and F1's
  <span class="header-icons">

  </span>
  </h3>
  <div class="step-content">
    <p class="description">Each season the crossing block is started by
      the Breeder's who select which of the F1's from the previous crossing
      block should re-enter the crossing block, as well as, additional
      germplasm to be used as parents in the new crosses.</p>
    <div id="step-pages" class="step-1">
      <ul>
        <li><a href="#step-forms">Forms</a></li>
        <li><a href="#step-lists">Data Listings</a></li>
      </ul>
      <div id="step-forms">
          <ul>
            <li><?php print l("Set Parents", $form_paths['parents']); ?><br />
              provides Breeders with a UI for indicating the new germplasm
              to be added to the crossing block</li>
            <li><?php print l("F1's Form", $form_paths['F1']); ?><br />
              lists all F1's from the previous crossing block, allowing breeder's
              to choose which should re-enter the crossing block and which
              should be grown out.</li>
          </ul>
      </div>
      <div id="step-lists">
          <ul>
            <li>Seed List<br />
              lists all the parents and F1's to go into this crossing block including
              the seed source entered by the breeder's to aid in gathering the material
              for the crossing block</li>
            <li>Labels Template<br />
              provides a listing of all the parent's and F1's for making labels to tag the
              various plants in the crossing block</li>
            <li>Crosses<br />
              a list of the crosses to be done for this crossing block in both a simplified
              version for use in the field and an expanded version listing all the details
              including pedigree of parents</li>
            <li>Grow-outs<br />
              a list of all the F1's to grow out including additional notes from the breeder</li>
          </ul>
      </div>
    </div>
  </div>

  <h3 class="step field-lab">
    <?php if ($form_steps['step2']['complete']) { print $checkmark_img_html; } ?>
    Screen the seed for Markers & Plant the crossing block
  </h3>
  <div class="step-content">
    <p class="description">Next members of the field lab will collect all
      the seed needed for the current crossing block based on the sources
      specified by the breeder's in the previous step. Ideally these seeds
      will be screened for the markers specified by the breeder and then
      only those with the markers of interest will be planted in the next step.
      Once the specific seeds have been chosen for the crossing block,
      they are planted in the field or greenhouse.</p>
    <div id="step-pages" class="step-2">
      <ul>
        <li><a href="#step-forms">Forms</a></li>
        <li><a href="#step-lists">Data Listings</a></li>
      </ul>
      <div id="step-forms">
          <ul>
            <li><?php print l("Indicate Marker's to be Screened with", $form_paths['parents']); ?><br />
              when the breeder's choose the parents/F1's for the crossing block
              they also have the option to specify which markers should be screened with.</li>
            <li><?php print l("Marker Data Entry", $form_paths['marker']); ?><br />
              marker screening results are entered in this form to make
              them available to the breeder's for use in finalizing the crossing
              block. This marker data is also available throughout KnowPulse.</li>
          </ul>
      </div>
      <div id="step-lists">
          <ul>
            <li>Marker Screening<br />
              lists all the germplasm to be screened indicating which markers each should
              be screened with.</li>
            <li>Marker Data<br />
              lists all the marker data currently entered in a seed set by marker grid. This
              listing is also available through the "Marker Data" tab on the Planning Forms.</li>
          </ul>
      </div>
    </div>
  </div>

  <h3 class="step breeder">
    <?php if ($form_steps['step3']['complete']) { print $checkmark_img_html; } ?>
    Make the Crosses
  </h3>
  <div class="step-content">
    <p class="description">When it nears time for the plants to flower,
      the breeder will finalize the crosses to be made. Designing of the
      crossing block is a continued process up until this point since
      additional data is collected at each previous step which needs to be
      taken into account.</p>
    <p class="description">Once the parents flower, the field lab crew
      makes the crosses as specified by the breeder. Each flower is tagged
      to indicate the cross made to ensure the pedigree of the seed produced
      is known.</p>
    <div id="step-pages" class="step-3">
      <ul>
        <li><a href="#step-forms">Forms</a></li>
        <li><a href="#step-lists">Data Listings</a></li>
      </ul>
      <div id="step-forms">
          <ul>
            <li><?php print l("Parents to Cross", $form_paths['parents']); ?><br />
              use the original planning clossing block parents form to further
              indicate the crosses which should be made between parents.</li>
            <li><?php print l("F1's to Cross", $form_paths['F1']); ?><br />
              use the original planning clossing block F1's form to indicate
              the parents which should be crossed with last crossing block's F1's.</li>
          </ul>
      </div>
      <div id="step-lists">
          <ul>
            <li>Crosses<br />
              a list of the crosses to be done for this crossing block in both a simplified
              version for use in the field and an expanded version listing all the details
              including pedigree of parents</li>
          </ul>
      </div>
    </div>
  </div>

  <h3 class="step field-lab">
    <?php if ($form_steps['step4']['complete']) { print $checkmark_img_html; } ?>
    Collect Seed & Record Details of Successful Crosses
  </h3>
  <div class="step-content">
    <p class="description">Once the maternal parents reach maturity, any seed
      produced is carefully collected. A cross number is assigned to each
      batch of seed resulting from a single cross to facillitate it's use
      & identification in future crossing blocks. These cross numbers are
      those in the F1 form (Plan Crosses above) for the next crossing block.</p>
    <div id="step-pages" class="step-4">
      <ul>
        <li><a href="#step-forms">Forms</a></li>
        <li><a href="#step-lists">Data Listings</a></li>
      </ul>
      <div id="step-forms">
          <ul>
            <li><?php print l("Describe Progeny", $form_paths['progeny']); ?><br />
              assign cross numbers to the seeds resulting from the crosses.
              Additional details are also recorded such as the number of seeds
              produced, etc.</li>
            <li>Indicate Failed Crosses<br />
              a form for indicating which crosses did not produce seed for complete record
              keeping</li>
          </ul>
      </div>
      <div id="step-lists">
          <ul>
            <li>Progeny<br />
              listing of all the successful crosses including cross number (once entered)
              and additional details. This is a good summary of the finished crossing block</li>
            <li>Failed Crosses<br />
              lists all the crosses marked as failed and any information entered describing
              how they failed or why.</li>
          </ul>
      </div>
    </div>
  </div>

</div>
