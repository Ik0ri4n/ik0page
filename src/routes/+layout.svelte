<script>
	import Header from '$lib/components/Header.svelte';
	import Toaster from '$lib/components/Toaster.svelte';
	import '$lib/styles/app.css';
</script>

<Header />

<main class="max-w-7xl mx-auto mt-4 px-4 sm:px-6 md:px-8">
	<div class="">
		<slot />
	</div>
</main>

<div class="fixed bottom-0 left-0 right-0 pointer-events-none">
	<Toaster />
</div>