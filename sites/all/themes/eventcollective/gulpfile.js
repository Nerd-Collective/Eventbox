/**
 * @file
 * Defines gulp tasks to be run by Gulp task runner.
 */

var gulp = require('gulp');
var sass = require('gulp-sass');
var sourcemaps = require('gulp-sourcemaps');
var chalk = require('chalk');
// var browserSync = require('browser-sync').create();
var autoprefixer = require('gulp-autoprefixer');

var input = 'scss/style.scss';
var output = './css/';

// Watch task
gulp.task('watch', function () {
    'use strict';
    gulp.watch('scss/**/*.scss', ['sass']);
  });

// Compile sass into CSS & auto-inject into browsers //
gulp.task('sass', function () {
    'use strict';
    return gulp
      .src(input)
      .pipe(sourcemaps.init())
      .pipe(sass().on('error', sass.logError))
      .pipe(autoprefixer('last 2 versions'))
      .pipe(sourcemaps.write('maps/'))
      .pipe(gulp.dest(output));
  });

// Add Support for Browsersync + watching scss/html files //
gulp.task('browsersync', ['sass'], function () {
    'use strict';
    browserSync.init({
      proxy: "ec.dev",
      reloadDelay: 1000
    });
    gulp.watch("scss/**/*.scss", ['sass']).on('change', browserSync.reload);
  });

gulp.task('default', ['sass', 'watch'], function () {
    console.log(chalk.bgGreen("....Gulp for Event Collective - http://ev.dev - commencing"));
});

